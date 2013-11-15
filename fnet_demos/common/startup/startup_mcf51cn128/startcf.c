/*
 *    CF_Startup.c - Default init/startup/termination routines for
 *                     Embedded Metrowerks C++
 *
 *    Copyright � 1993-1998 Metrowerks, Inc. All Rights Reserved.
 *    Copyright � 2005 Freescale semiConductor Inc. All Rights Reserved.
 *
 *
 *    THEORY OF OPERATION
 *
 *    This version of thestartup code is intended for linker relocated
 *    executables.  The startup code will assign the stack pointer to
 *    __SP_INIT, assign the address of the data relative base address
 *    to a5, initialize the .bss/.sbss sections to zero, call any
 *    static C++ initializers and then call main.  Upon returning from
 *    main it will call C++ destructors and call exit to terminate.
 */

#ifdef __cplusplus
#pragma cplusplus off
#endif
#pragma PID off
#pragma PIC off

#include "startcf.h"

/* copy ROM to RAM locations.  Set to 0 for more aggressive dead stripping ... */
#ifndef SUPPORT_ROM_TO_RAM
#define SUPPORT_ROM_TO_RAM			1
#endif

	/* imported data */

extern unsigned long far _SP_INIT, _SDA_BASE;
extern unsigned long far _START_BSS, _END_BSS;
extern unsigned long far _START_SBSS, _END_SBSS;
extern unsigned long far __DATA_RAM, __DATA_ROM, __DATA_END;

	/* imported routines */

extern void __call_static_initializers(void);
extern int main(int, char **);
extern void exit(int);

	/* exported routines */

extern void _ExitProcess(void);
extern asm void _startup(void);
extern void __initialize_hardware(void);
extern void __initialize_system(void);


/*
 *    Dummy routine for initializing hardware.  For user's custom systems, you
 *    can create your own routine of the same name that will perform HW
 *    initialization.  The linker will do the right thing to ignore this
 *    definition and use the version in your file.
 */
#pragma overload void __initialize_hardware(void);
void __initialize_hardware(void)
{
}

/*
 *    Dummy routine for initializing systems.  For user's custom systems,
 *    you can create your own routine of the same name that will perform
 *    initialization.  The linker will do the right thing to ignore this
 *    definition and use the version in your file.
 */
#pragma overload void __initialize_system(void);
void __initialize_system(void)
{
}

/*
 *    Dummy routine for initializing C++.  This routine will get overloaded by the C++ runtime.
 */
#pragma overload void __call_static_initializers(void);
void __call_static_initializers(void)
{
}

/*
 *	Routine to copy a single section from ROM to RAM ...
 */
static __declspec(register_abi) void __copy_rom_section(char* dst, const char* src, unsigned long size)
{
	if (dst != src)
		 while (size--)
		    *dst++ = *src++;
}

/*
 *	Routine that copies all sections the user marked as ROM into
 *	their target RAM addresses ...
 *
 *	__S_romp is automatically generated by the linker if it
 *	is referenced by the program.  It is a table of RomInfo
 *	structures.  The final entry in the table has all-zero
 *	fields.
 */
static void __copy_rom_sections_to_ram(void)
{
	RomInfo		*info;

	/*
	 *	Go through the entire table, copying sections from ROM to RAM.
	 */
	for (info = _S_romp; info->Source != 0L || info->Target != 0L || info->Size != 0; ++info)
    __copy_rom_section( (char *)info->Target,(char *)info->Source, info->Size);
							
}

/*
 *    Exit handler called from the exit routine, if your OS needs
 *    to do something special for exit handling just replace this
 *    routines with what the OS needs to do ...
 */
__declspec(register_abi) asm void _ExitProcess(void)
{
	illegal
	rts
}

/*
 *    Routine to clear out blocks of memory should give good
 *    performance regardless of 68k or ColdFire part.
 */
static __declspec(register_abi) void clear_mem(char *dst, unsigned long n)
{
	unsigned long i;
	long *lptr;

	if (n >= 32)
	{
		/* align start address to a 4 byte boundary */
		i = (- (unsigned long) dst) & 3;

		if (i)
		{
			n -= i;
			do
				*dst++ = 0;
			while (--i);
		}

		/* use an unrolled loop to zero out 32byte blocks */
		i = n >> 5;
		if (i)
		{
			lptr = (long *)dst;
			dst += i * 32;
			do
			{
				*lptr++ = 0;
				*lptr++ = 0;
				*lptr++ = 0;
				*lptr++ = 0;
				*lptr++ = 0;
				*lptr++ = 0;
				*lptr++ = 0;
				*lptr++ = 0;
			}
			while (--i);
		}
		i = (n & 31) >> 2;

		/* handle any 4 byte blocks left */
		if (i)
		{
			lptr = (long *)dst;
			dst += i * 4;
			do
				*lptr++ = 0;
			while (--i);
		}
		n &= 3;
	}

	/* handle any byte blocks left */
	if (n)
		do
			*dst++ = 0;
		while (--n);
}

/*
 *    Startup routine for embedded application ...
 */

__declspec(register_abi) asm void _startup(void)
{
	/* disable interrupts */
    move.w        #0x2700,sr

	/* Pre-init SP, in case memory for stack is not valid it should be setup using 
	   MEMORY_INIT before __initialize_hardware is called 
	*/
	lea __SP_AFTER_RESET,a7; 

    /* initialize memory */
    MEMORY_INIT

	/* initialize any hardware specific issues */
    jsr           __initialize_hardware   
  
	/* setup the stack pointer */
    lea           _SP_INIT,a7

	/* setup A6 dummy stackframe */
    movea.l       #0,a6
    link          a6,#0

	/* setup A5 */
    lea           _SDA_BASE,a5


	/* zero initialize the .bss section */

    lea           _END_BSS, a0
    lea           _START_BSS, a1
    suba.l        a1, a0
    move.l        a0, d0

    beq           __skip_bss__

    lea           _START_BSS, a0

    /* call clear_mem with base pointer in a0 and size in d0 */
    jsr           clear_mem

__skip_bss__:

	/* zero initialize the .sbss section */

    lea           _END_SBSS, a0
    lea           _START_SBSS, a1
    suba.l        a1, a0
    move.l        a0, d0

    beq           __skip_sbss__

    lea           _START_SBSS, a0

    /* call clear_mem with base pointer in a0 and size in d0 */
    jsr           clear_mem

__skip_sbss__:

	/* copy all ROM sections to their RAM locations ... */
#if SUPPORT_ROM_TO_RAM

	/*
	 * _S_romp is a null terminated array of
	 * typedef struct RomInfo {
     *      unsigned long	Source;
     *      unsigned long	Target;
     *      unsigned long 	Size;
     *  } RomInfo;
     *
     * Watch out if you're rebasing using _PICPID_DELTA
     */

    lea           _S_romp, a0
    move.l        a0, d0
    beq           __skip_rom_copy__            
    jsr           __copy_rom_sections_to_ram

#else

	/*
   * There's a single block to copy from ROM to RAM, perform
   * the copy directly without using the __S_romp structure
   */

    lea           __DATA_RAM, a0
    lea           __DATA_ROM, a1
    
    cmpa          a0,a1
    beq           __skip_rom_copy__
              
    move.l        #__DATA_END, d0
    sub.l         a0, d0
                  
    jsr           __copy_rom_section

#endif
__skip_rom_copy__:
	
	/* call C++ static initializers (__sinit__(void)) */
	jsr			  __call_static_initializers

	jsr		  	  __initialize_system

	/* call main(int, char **) */
	pea			  __argv
	clr.l		  -(sp)				/* clearing a long is ok since it's caller cleanup */
	jsr			  main
	addq.l		#8, sp
	
	unlk		  a6
	
	/* now call exit(0) to terminate the application */
/*	clr.l		  -(sp)
	jsr			  exit
	addq.l		#4, sp
 */
	/* should never reach here but just in case */
	illegal
	rts

	/* exit will never return */
__argv:
    dc.l          0
}

