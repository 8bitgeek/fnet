/**************************************************************************
* 
* Copyright 2011-2015 by Andrey Butok. FNET Community.
* Copyright 2008-2010 by Andrey Butok. Freescale Semiconductor, Inc.
*
***************************************************************************
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License Version 3 
* or later (the "LGPL").
*
* As a special exception, the copyright holders of the FNET project give you
* permission to link the FNET sources with independent modules to produce an
* executable, regardless of the license terms of these independent modules,
* and to copy and distribute the resulting executable under terms of your 
* choice, provided that you also meet, for each linked independent module,
* the terms and conditions of the license of that module.
* An independent module is a module which is not derived from or based 
* on this library. 
* If you modify the FNET sources, you may extend this exception 
* to your version of the FNET sources, but you are not obligated 
* to do so. If you do not wish to do so, delete this
* exception statement from your version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
* You should have received a copy of the GNU General Public License
* and the GNU Lesser General Public License along with this program.
* If not, see <http://www.gnu.org/licenses/>.
*
**********************************************************************/ /*!
*
* @file fnet_flash.c
*
* @author Andrey Butok
*
* @brief On-chip Flash Module driver.
*
***************************************************************************/
#include "fnet.h" 

#if FNET_CFG_FLASH

#define FNET_CFG_FLASH_WRITE_CACHE 0

#if FNET_CFG_FLASH_WRITE_CACHE /* Just prototype.*/
/**************************************************************************/ /*!
 * Flash cache entry.
 ******************************************************************************/
struct fnet_flash_cache_entry
{

    void            *dest_addr; /* Must be aligned to FNET_CFG_CPU_FLASH_PROGRAM_SIZE address.*/
    unsigned char   data[FNET_CFG_CPU_FLASH_PROGRAM_SIZE];
};

/* Write-cache.*/
static  struct fnet_flash_cache_entry flash_cache[FNET_CFG_FLASH_WRITE_CACHE_SIZE];
static  unsigned long   entry_next;

static void fnet_flash_cache_flush_entry(struct fnet_flash_cache_entry   *entry);

#endif

static void fnet_flash_write_low( unsigned char *dest, const unsigned char *src, unsigned int n_blocks );



#if FNET_CFG_FLASH_WRITE_CACHE
static void fnet_flash_cache_flush_entry(struct fnet_flash_cache_entry   *entry)
{
    if(entry && entry->dest_addr)
    {
        /* Actual write to the flash.*/
        fnet_flash_write_low( entry->dest_addr, entry->data, 1 );
        /* Clean up.*/
        fnet_memset(entry->data, 0xFF, FNET_CFG_CPU_FLASH_PROGRAM_SIZE);
        entry->dest_addr = 0;
    }
}

static void fnet_flash_cache_write(unsigned char *dest_addr, unsigned char *data_p)
{
    int                             i;
    struct fnet_flash_cache_entry   *entry = FNET_NULL;
    
    /* Find existing entry*/
    for(i=0; i<FNET_CFG_FLASH_WRITE_CACHE_SIZE; i++)
    {
        if(flash_cache[i].dest_addr == dest_addr);
        {
            entry = &flash_cache[i];
            break;
        }
    }

    if(entry == FNET_NULL) /* No existing, so use the current one.*/
    { 
        entry = &flash_cache[entry_next];

        entry_next++;
        if(entry_next == FNET_CFG_FLASH_WRITE_CACHE_SIZE)
        {
            entry_next = 0;
        }
        
        fnet_flash_cache_flush_entry(entry); /* Falsh if something in the cache entry.*/

        entry->dest_addr = dest_addr; /* Init destination address.*/
    }
    
    for(i=0; i<FNET_CFG_CPU_FLASH_PROGRAM_SIZE; i++)
    {
        entry->data[i] &= data_p[i];
    }
}

/************************************************************************
* NAME: fnet_flash_flush
*
* DESCRIPTION:
************************************************************************/
void fnet_flash_flush(void)
{
    int i;

    for(i=0; i<FNET_CFG_FLASH_WRITE_CACHE_SIZE; i++)
    {
        fnet_flash_cache_flush_entry(&flash_cache[i]);
    }
}


#endif


/************************************************************************
* NAME: fnet_flash_erase
*
* DESCRIPTION:
************************************************************************/
void fnet_flash_erase( void *flash_addr, unsigned bytes)
{
    unsigned long n_pages;
    
    unsigned long page_shift = (unsigned long)flash_addr & (FNET_CFG_CPU_FLASH_PAGE_SIZE - 1U);
    
    flash_addr = (void *)((unsigned long)flash_addr - page_shift);
    
    bytes += page_shift;
    
    n_pages = (unsigned long)( bytes/FNET_CFG_CPU_FLASH_PAGE_SIZE + (unsigned long)((bytes%FNET_CFG_CPU_FLASH_PAGE_SIZE)?1U:0U));

    while (n_pages)
    {
        fnet_cpu_flash_erase(flash_addr);
        flash_addr = (void *)((unsigned long)flash_addr + FNET_CFG_CPU_FLASH_PAGE_SIZE);
        n_pages --;
    }
}

/************************************************************************
* NAME: fnet_flash_write_low
*
* DESCRIPTION:
************************************************************************/
static void fnet_flash_write_low( unsigned char *dest, const unsigned char *src, unsigned int n_blocks  )
{
    while (n_blocks)
    {
        fnet_cpu_flash_write(dest, src);
        dest+= FNET_CFG_CPU_FLASH_PROGRAM_SIZE;
        src+=FNET_CFG_CPU_FLASH_PROGRAM_SIZE;
        n_blocks--;
    }
}

/************************************************************************
* NAME: fnet_flash_memcpy
*
* DESCRIPTION:
************************************************************************/

void fnet_flash_memcpy( FNET_COMP_PACKED_VAR void *flash_addr, FNET_COMP_PACKED_VAR const void *src, unsigned n )
{
    unsigned long   i;
    unsigned char   data[FNET_CFG_CPU_FLASH_PROGRAM_SIZE];    
    unsigned long   bytes;
    unsigned long   blocks;
    unsigned long   count;
    
    if(n)
    {
        count = (unsigned long)flash_addr & (FNET_CFG_CPU_FLASH_PROGRAM_SIZE-1U);
        
        /* Align dest. */
        if(count)
        {   
            flash_addr = (unsigned long *) ((unsigned long)flash_addr - count);
            bytes=FNET_CFG_CPU_FLASH_PROGRAM_SIZE-count;

            if(bytes > n)
                bytes = n;

            fnet_memset(data, 0xFF, FNET_CFG_CPU_FLASH_PROGRAM_SIZE);
            for(i = count; i<count+bytes; i++)
            {
                data[i] = *((const unsigned char *)src); 
                src = (unsigned char *) ((unsigned long)src + 1U);
            }
            
            fnet_flash_write_low( (unsigned char *)flash_addr, data, 1U );
            
            flash_addr = (unsigned char *) ((unsigned long)flash_addr + FNET_CFG_CPU_FLASH_PROGRAM_SIZE);
            
            n-=bytes;   
        }
        
        if(n)
        {
            bytes = n & (FNET_CFG_CPU_FLASH_PROGRAM_SIZE-1U);
		    blocks = (n - bytes) / FNET_CFG_CPU_FLASH_PROGRAM_SIZE;

            fnet_flash_write_low((unsigned char *)flash_addr, (const unsigned char *)src, blocks );
			
			flash_addr = (unsigned char *) ((unsigned long)flash_addr + (FNET_CFG_CPU_FLASH_PROGRAM_SIZE*blocks));
			src = (unsigned char *) ((unsigned long)src + (FNET_CFG_CPU_FLASH_PROGRAM_SIZE*blocks));
            
            if(bytes)
            {
                fnet_memset(data, 0xFF, FNET_CFG_CPU_FLASH_PROGRAM_SIZE);
                
                for(i=0U;i<bytes;i++)
                {
                    data[i] = *((const unsigned char *)src); 
                    src = (unsigned char *) ((unsigned long)src + 1U);
                }

    		    fnet_flash_write_low((unsigned char *)flash_addr, data, 1U );
            }
        }
    }
}

#endif /* FNET_CFG_FLASH */
