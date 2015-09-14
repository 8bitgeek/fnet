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
* @file fnet_mempool.h
*
* @author Andrey Butok
*
* @brief FNET memory pools API.
*
***************************************************************************/

#ifndef _FNET_MEMPOOL_H_

#define _FNET_MEMPOOL_H_

#include "fnet.h"

/**************************************************************************/ /*!
 * @internal
 * @brief Memory pool descriptor.
 * @see fnet_mempool_init()
 ******************************************************************************/
typedef fnet_int32_t fnet_mempool_desc_t;

/* Memory pool unit header.*/
FNET_COMP_PACKED_BEGIN
typedef struct fnet_mempool_unit_header 
{
    fnet_size_t                         size FNET_COMP_PACKED;                     /* Unit size. */
    struct fnet_mempool_unit_header     *ptr FNET_COMP_PACKED;   /* Pointer to the next free block. */
} 
fnet_mempool_unit_header_t;
FNET_COMP_PACKED_END


typedef enum
{
    FNET_MEMPOOL_ALIGN_8 = (0x7u),       /* Evenly divisible by 8.*/
    FNET_MEMPOOL_ALIGN_16 = (0xFu),      /* Evenly divisible by 16.*/
    FNET_MEMPOOL_ALIGN_32 = (0x1Fu),     /* Evenly divisible by 32.*/
    FNET_MEMPOOL_ALIGN_64 = (0x3Fu)      /* Evenly divisible by 64.*/
}
fnet_mempool_align_t;

#if defined(__cplusplus)
extern "C" {
#endif

fnet_mempool_desc_t fnet_mempool_init( void *pool_ptr, fnet_size_t pool_size, fnet_mempool_align_t alignment );
void fnet_mempool_release( fnet_mempool_desc_t mpool );
void fnet_mempool_free( fnet_mempool_desc_t mpool, void *ap );
void *fnet_mempool_malloc(fnet_mempool_desc_t mpool, fnet_size_t nbytes );
fnet_size_t fnet_mempool_free_mem_status( fnet_mempool_desc_t mpool);
fnet_size_t fnet_mempool_malloc_max( fnet_mempool_desc_t mpool );

#if 0 /* For Debug needs.*/
fnet_return_t fnet_mempool_check( fnet_mempool_desc_t mpool );
#endif

#if defined(__cplusplus)
}
#endif

#endif
