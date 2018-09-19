//
//CONFIDENTIAL AND PROPRIETARY INFORMATION
//
//Copyright (c) 2012 Synopsys, Inc. All rights reserved. 
//This software and documentation contain confidential and
//proprietary information that is the property of 
//Synopsys, Inc. The software and documentation are 
//furnished under a license agreement and may be used 
//or copied only in accordance with the terms of the license 
//agreement. No part of the software and documentation 
//may be reproduced, transmitted, or translated, in any 
//form or by any means, electronic, mechanical, manual, 
//optical, or otherwise, without prior written permission 
//of Synopsys, Inc., or as expressly provided by the license agreement.
//Reverse engineering is prohibited, and reproduction, 
//disclosure or use without specific written authorization
//of Synopsys Inc. is strictly forbidden.
//

 



#include <stdio.h>
#include <memory.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdarg.h>                 /* va_start, va_end support     */
#include "helper_lib.h"
#include "testlib_types.h"

#ifdef _ARC
#if _ARCVER > 0x39 
    #include <arc/arc_reg.h>
#else
    #include "platform_lib/arc.h"

	#define REG_DC_RAM_ADDR                      (0x058)
	#define REG_DC_TAG                           (0x059)
	#define REG_D_CACHE_BUILD                    (0x072)
#endif

#define PSP_AUX_DC_TAG_VALID_FLAG                (0x00000001)
#define PSP_AUX_DC_TAG_LOCK_FLAG                 (0x00000002)
#define PSP_AUX_DC_TAG_DIRTY_FLAG                (0x00000004)

#define D_CACHE_SIZE_MASK                        (0x0000F000)

#define XY_MEMORY_SIZE              (8192)
#endif

#ifndef DCACHE_DISABLED
  #ifndef CACHE_LINE_SIZE
    #error "CACHE_LINE_SIZE is not defined. It must be defined in the makefile. If it is not required in your component just set it to the default value 32 (bytes)."
  #else
    #define MEMORY_ALIGNMENT          (CACHE_LINE_SIZE-1)
    #define MEMORY_ALIGNMENT_MASK     (~MEMORY_ALIGNMENT)
  #endif /* ifndef CACHE_LINE_SIZE */
#endif  /*DCACHE_DISABLED*/


#ifndef DCACHE_DISABLED
static uint32_t get_dcache_size()
{
  uint32_t dcache_size_config = (_lr(REG_D_CACHE_BUILD) & D_CACHE_SIZE_MASK) >> 12;
  // 512 bytes only supported in ARC600
  return (dcache_size_config == 0) ? 512 : ((1 << (dcache_size_config - 1)) << 10);
}
#endif

/* 
 *  Aligned memory allocation implementation - needed for buffer allocation
 */

static void* malloc_aligned(uint32_t size) 
{
    void *orig_ptr;
    void *aligned_ptr;
    void **orig_ptr_save_ptr;

#ifndef DCACHE_DISABLED

    orig_ptr = malloc(size + CACHE_LINE_SIZE + sizeof(void*));

    if(orig_ptr == NULL)
        return NULL;

    aligned_ptr = (void*)(((uint32_t)orig_ptr + CACHE_LINE_SIZE + sizeof(void*)) & MEMORY_ALIGNMENT_MASK);

    /* Saving original address into previous word */
    orig_ptr_save_ptr = (void**)((uint32_t)aligned_ptr - sizeof(void*));

    *orig_ptr_save_ptr = orig_ptr;

#else

    orig_ptr = malloc(size + sizeof(void*));

    if(orig_ptr == NULL)
        return NULL;

    aligned_ptr = (void*)(((uint32_t)orig_ptr + sizeof(void*)));

    /* Saving original address into previous word */
    orig_ptr_save_ptr = (void**)((uint32_t)aligned_ptr - sizeof(void*));

    *orig_ptr_save_ptr = orig_ptr;

#endif

    return aligned_ptr;
}


static void free_aligned(void *ptr) 
{
    void* orig_ptr;
    void **orig_ptr_save_ptr;

    TEST_ASSERT(ptr != NULL);

    orig_ptr_save_ptr = (void**)((uint32_t)ptr - sizeof(void*));

    orig_ptr = *orig_ptr_save_ptr;

    free(orig_ptr);
}


/*
  debug print function for codecs supporting
  ARC Audio API
*/
void debug_print( const char* format, ...)
{
    int32_t len;
    va_list argp;

    va_start(argp, format);
    len = vprintf(format, argp);
    va_end(argp);
}


/*
  Memory allocation function for codecs supporting ARC Audio API
Note:
  Current implementation of the helper library ignores the
  'cacheable' parameter. This is due to ARC hardware limitations
  This parameter can be used in other platforms or in future ARC
  platforms to support allocation of memory which can be accesed
  bypassing the cache
*/
#if defined NATIVE_STRESS_TEST || defined NATIVE_CYCLE_PROFILING
    static Application_Settings_t *settings = NULL;
#endif

void* mem_alloc(uint32_t size)
{
    void *buffer;
    DBG("mem_malloc=%u\n", size);
#ifdef RT_CHECK_ZERO_LENGTH_MEM_FUNCTIONS
    
    if (size ==0) 
    {
        fprintf(stderr, "[RT Check Error]: An attempt to allocate 0-length memory block (RAM) has been made" NEWLINE);
        return NULL;
    }
#endif

    /* When a codec and helper library are compiled in profiling mode   */
    /* the hAppContext cannot be NULL since it contains the heap and    */
    /* stack usage counters. Therefore, hAppContext is checked for NULL */
    /* and if the check fails, then mem_alloc() returns an error. This  */
    /* is not API compliant. It is used only for profiling. Make sure   */
    /* that this section is undefined when running the conformance test */
    /* The same is for STRESS TEST. hAppContext is required for stress  */
    /* testing                                                          */
#ifdef NATIVE_STRESS_TEST
    /* stress testing support */
    DBG("NATIVE_STRESS_TEST=%u, %u",  settings->malloc_counter, settings->stress_test_counter);

    if (settings != NULL && settings->f_stress_test_enabled == 1)
    {
        settings->malloc_counter++;
        if (settings->malloc_counter == settings->stress_test_counter)
        {
            return NULL;
        }
    }
#endif

    buffer = malloc_aligned(size);

    if (buffer == NULL) 
    {
        return buffer;
    }

#if defined NATIVE_STRESS_TEST || defined NATIVE_CYCLE_PROFILING
    if (settings != NULL) {
    	settings->memory_stats.heap_usage += _msize(*((void**)((uint32_t)buffer - sizeof(void*))));
    	settings->memory_stats.max_heap_usage = _max(settings->memory_stats.max_heap_usage, settings->memory_stats.heap_usage);
    }
#endif
    return buffer;
} /* mem_alloc */

void mem_set_context(void *app)
{
#if defined NATIVE_STRESS_TEST || defined NATIVE_CYCLE_PROFILING
	settings = app;
#endif
}

/*
  Memory freeing function for codecs supporting
  ARC Audio API
*/
void mem_free(void *ptr)
{
    TEST_ASSERT(ptr != NULL);
#if defined NATIVE_STRESS_TEST || defined NATIVE_CYCLE_PROFILING
	if (settings != NULL) {
		settings->memory_stats.heap_usage -= _msize(*((void**)((uint32_t)ptr - sizeof(void*))));
	}
#endif
    free_aligned(ptr);
} /* mem_free */


/* Cache invalidation routine. It works if flush-on-invalidate is turned off! */
void cache_invalidate(void* address,
                      uint32_t size)
{
#ifdef RT_CHECK_ZERO_LENGTH_CACHE_FUNCTIONS
    
    if (size == 0) 
    {
        fprintf(stderr, "[RT Check Error]: An attempt to invalidate a 0-length d-cache block at 0x%08X has been made" NEWLINE, (uint32_t)address);
    }

#endif
    TEST_ASSERT(address != NULL);
    TEST_ASSERT(size != 0);

#ifndef DCACHE_DISABLED
	uint32_t start_address, final_address, current_address, dcache_size;
    int32_t i;
	
    // Calculate the start and final addresses of the buffer space
    start_address = ((uint32_t)address) & MEMORY_ALIGNMENT_MASK;
    final_address = ((uint32_t)address) + size - 1;
    dcache_size =  get_dcache_size();  
	
    if(size >= 4*dcache_size){
        // Iterate over D$ lines. This method gives performance benefits for really big buffers
        // (according to XCAM profiling this becomes faster on 4x of D$ size

        uint32_t cache_line, tag;
        // Backup cache control register
        uint32_t saved_at = _lr(REG_DC_CTRL);

        // Reset the AT flag
        _sr(saved_at & ~REG_DC_CTRL_AT_BIT, REG_DC_CTRL);

        // Loop counter is the cache RAM line address
        for (cache_line = 0; cache_line < dcache_size; cache_line+=CACHE_LINE_SIZE) {
            // Set the address into appropriated cache register
            _sr( cache_line, REG_DC_RAM_ADDR);

            // Get the tag from the cache TAG register
            tag = _lr( REG_DC_TAG );

            // Analyse V and lck bits. If the line is invalid or locked then go to new iteration
            if ((tag & (PSP_AUX_DC_TAG_VALID_FLAG | PSP_AUX_DC_TAG_LOCK_FLAG )) == (PSP_AUX_DC_TAG_VALID_FLAG)) {
                // Generate address of RAM
                current_address = tag & MEMORY_ALIGNMENT_MASK;

                // Check whether that address is in the buffer address space. If not then go to next iteration
                if ((current_address >= start_address) && (current_address <= final_address)) {
                    _sr(current_address, REG_DC_IVDL);
                }
            }
        }

        // Restore the AT flag
        saved_at &= REG_DC_CTRL_AT_BIT;
        saved_at |= _lr(REG_DC_CTRL);
        _sr(saved_at, REG_DC_CTRL);
    } else {
        // Iterate over buffer lines
        for (i = (final_address - start_address)/CACHE_LINE_SIZE; i >= 0; i-- )
        {
            _sr(start_address, REG_DC_IVDL);
            start_address += CACHE_LINE_SIZE;
        }
    }

#endif /* ifndef DCACHE_DISABLED */
} /* cache_invalidate */


void cache_flush(void* address,
                 uint32_t size)
{


#ifdef RT_CHECK_ZERO_LENGTH_CACHE_FUNCTIONS

    if (size == 0) 
    {
        fprintf(stderr, "[RT Check Error]: An attempt to flush a 0-length d-cache block at 0x%08X has been made" NEWLINE, (uint32_t)address);
    }

#endif

    TEST_ASSERT(address != NULL);
    TEST_ASSERT(size != 0);

#ifndef DCACHE_DISABLED
    uint32_t start_address, final_address, current_address, dcache_size;
    int32_t i;

    // Calculate the start and final addresses of the buffer space
    start_address = ((uint32_t)address) & MEMORY_ALIGNMENT_MASK;
    final_address = ((uint32_t)address) + size - 1;
    dcache_size =  get_dcache_size();

    if(size >= 4*dcache_size){
        // Iterate over D$ lines. This method gives performance benefits for really big buffers
        // (according to XCAM profiling this becomes fasted on 4x of D$ size

        uint32_t cache_line, tag;
        // Backup cache control register
        uint32_t saved_at = _lr(REG_DC_CTRL);

        // Reset the AT flag
        _sr(saved_at & ~REG_DC_CTRL_AT_BIT, REG_DC_CTRL);

        // Loop counter is the cache RAM line address
        for (cache_line = 0; cache_line < dcache_size; cache_line+=CACHE_LINE_SIZE) {
            // Set the address into appropriated cache register
            _sr( cache_line, REG_DC_RAM_ADDR);

            // Get the tag from the cache TAG register
            tag = _lr( REG_DC_TAG );

            // Analyse V and lck bits. If the line is invalid or locked then go to new iteration
            if ((tag & (PSP_AUX_DC_TAG_VALID_FLAG | PSP_AUX_DC_TAG_LOCK_FLAG | PSP_AUX_DC_TAG_DIRTY_FLAG)) == (PSP_AUX_DC_TAG_VALID_FLAG|PSP_AUX_DC_TAG_DIRTY_FLAG)) {
                // Generate address of RAM
                current_address = tag & MEMORY_ALIGNMENT_MASK;

                // Check whether that address is in the buffer address space. If not then go to next iteration
                if ((current_address >= start_address) && (current_address <= final_address)) {
                    _sr(current_address, REG_DC_FLDL);
                }
            }
        }

        // Restore the AT flag
        saved_at &= REG_DC_CTRL_AT_BIT;
        saved_at |= _lr(REG_DC_CTRL);
        _sr(saved_at, REG_DC_CTRL);
    } else {
        // Iterate over buffer lines
        for (i = (final_address - start_address)/CACHE_LINE_SIZE; i >= 0; i-- )
        {
            /* Next operation will stall the core if flush buffer is already busy.
               This feature allows to eliminate DC_CTRL polling for every line.
               Instead it's necessary to poll DC_CTRL only for the last line */
            _sr(start_address, REG_DC_FLDL);
            start_address += CACHE_LINE_SIZE;
        }
    }

    /* wait for flush completion */
    while(_lr(REG_DC_CTRL) & REG_DC_CTRL_FS_BIT);
    
#endif /* ifndef DCACHE_DISABLED */

} /* cache_flush */


void cache_flush_invalidate(void* address,
                            uint32_t size)
{
#ifdef RT_CHECK_ZERO_LENGTH_CACHE_FUNCTIONS
    
    if (size == 0) 
    {
        fprintf(stderr, "ERROR: An attempt to flush+invalidate a 0-length d-cache block at 0x%08X has been made" NEWLINE, (uint32_t)address);
    }

#endif

    TEST_ASSERT(address != NULL);
    TEST_ASSERT(size != 0);

#ifndef DCACHE_DISABLED

    uint32_t saved_ctrl;

    // Backup cache control register
    saved_ctrl = _lr(REG_DC_CTRL);
    _sr(saved_ctrl|REG_DC_CTRL_IM_BIT, REG_DC_CTRL);

    cache_invalidate(address, size);

    _sr(saved_ctrl, REG_DC_CTRL);

    /* wait for flush completion */
    while(_lr(REG_DC_CTRL) & REG_DC_CTRL_FS_BIT);

#endif /* ifndef DCACHE_DISABLED */
} /* cache_flush_invalidate */

#ifndef DCACHE_DISABLED
#define PSP_AUX_DC_CTRL_CACHE_CONTROL_RAM_ACCESS (0x00000020)
#define PSP_AUX_DC_RAM_ADDR                      (0x058)
#define PSP_AUX_DC_TAG                           (0x059)
#define PSP_AUX_DC_TAG_VALID_FLAG                (0x00000001)
#define PSP_AUX_DC_TAG_LOCK_FLAG                 (0x00000002)
#define PSP_AUX_DC_TAG_DIRTY_FLAG                (0x00000004)
#define RT_CHECK_DCACHE_MEMORY_ALIGNMENT         (CACHE_LINE_SIZE-1)
#define RT_CHECK_DCACHE_MEMORY_ALIGNMENT_MASK    (~RT_CHECK_DCACHE_MEMORY_ALIGNMENT)
#define RT_CHECK_DCACHE_SIZE                     (16*1024)
#define PSP_AUX_DC_CTRL                          (0x048)
#define PSP_AUX_DC_BUILD                     (0x072)
#define PSP_AUX_DC_CTRL_ENABLE_BYPASS            (0x00000002)
#endif

uint32_t check_dcache_dirty(void *buffer, uint32_t size)
{
        DBG0();
#ifndef DCACHE_DISABLED
	DBG0();
    uint32_t start_address, final_address, current_address;
    uint32_t cache_line, tag;
    uint32_t saved_at;
	if ((_lr(PSP_AUX_DC_BUILD) & 0x00300000) != 0x00200000) {
		WRN("CHECK_DCACHE is only supported for DC_FEATURE_LEVEL=2, but lr=%u!", _lr(PSP_AUX_DC_BUILD));
		return TEST_TRUE;
	}

    // Calculate the start and final addresses of the buffer space
    start_address = ((uint32_t)buffer) & RT_CHECK_DCACHE_MEMORY_ALIGNMENT_MASK;
    final_address = ((uint32_t)buffer) + size - 1;

    // Reset the AT flag and store it in the saved_at
    saved_at = _lr(PSP_AUX_DC_CTRL);
    _sr(saved_at & ~PSP_AUX_DC_CTRL_CACHE_CONTROL_RAM_ACCESS, PSP_AUX_DC_CTRL);

    saved_at &= PSP_AUX_DC_CTRL_CACHE_CONTROL_RAM_ACCESS;

    // Loop counter is the cache RAM line address
    for (cache_line = 0; cache_line < RT_CHECK_DCACHE_SIZE; cache_line+=CACHE_LINE_SIZE)
    {
        // Set the address into appropriated cache register
        _sr( cache_line, PSP_AUX_DC_RAM_ADDR);

        // Get the tag from the cache TAG register
        tag = _lr( PSP_AUX_DC_TAG );

        // Analyse V and lck bits. If the line is invalid or locked then go to new iteration
        if ( (tag & (PSP_AUX_DC_TAG_VALID_FLAG | PSP_AUX_DC_TAG_LOCK_FLAG | PSP_AUX_DC_TAG_DIRTY_FLAG)) == (PSP_AUX_DC_TAG_VALID_FLAG|PSP_AUX_DC_TAG_DIRTY_FLAG) )
        {
            // Generate address of RAM
            current_address = tag & RT_CHECK_DCACHE_MEMORY_ALIGNMENT_MASK;

            // Check whether that address is in the buffer address space. If not then go to new iteration
            if (current_address >= start_address && current_address <= final_address)
            {
                uint32_t tmp;

                tmp =_lr(PSP_AUX_DC_CTRL);
                saved_at |= tmp;
                _sr(saved_at, PSP_AUX_DC_CTRL);

                //Buffer is dirty, fail the test
                WRN("Data cache 'dirty data' check has failed for buffer (addr=0x%X size=%d) at address 0x%X" , (uint32_t)buffer, size, current_address);
                return TEST_FALSE;
            }
        }
    }

    // Restore the AT flag
    {
        uint32_t tmp;

        tmp = _lr(PSP_AUX_DC_CTRL);
        saved_at |= tmp;
        _sr(saved_at, PSP_AUX_DC_CTRL);
    }
#endif
    return TEST_TRUE;
}
