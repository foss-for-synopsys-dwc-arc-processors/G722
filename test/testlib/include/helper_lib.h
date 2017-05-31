/*
// CONFIDENTIAL AND PROPRIETARY INFORMATION
// All trademarks and registered trademarks are the property of
// Virage Logic Corporation or their respective owners and are
// protected herein. All Rights Reserved.
// Copyright 2003-2011 Virage Logic Corporation
//
// This document, material and/or software contains confidential
// and proprietary information of Virage Logic and is
// protected by copyright, trade secret and other state, federal,
// and international laws, and may be embodied in patents issued
// or pending.  Its receipt or possession does not convey any
// rights to use, reproduce, disclose its contents, or to
// manufacture, or sell anything it may describe.  Reverse
// engineering is prohibited, and reproduction, disclosure or use
// without specific written authorization of Virage Logic is
// strictly forbidden.
//
// ARC Product:  Helper library for ARC Audio API 1.0
// File version: $Revision$
//
// Description:
//
//
//
// Author:
//
*/

#ifndef _TEST_HELPER_LIB_H_
#define _TEST_HELPER_LIB_H_

#include <stdint.h>
#define mem_alloc mem_alloc_1_0
#define mem_free mem_free_1_0


// get CACHE configuration from TCF file and
// translate it into TEST defines
#undef CACHE_LINE_SIZE
#undef DCACHE_DISABLED
#if defined __core_config_h && (core_config_dcache_present==1)
#define CACHE_LINE_SIZE core_config_dcache_line_size
#else
#define DCACHE_DISABLED
#endif

/* Function names replacement section - required 
   to support several Helper Libraries linkage into single binary */
uint32_t check_dcache_dirty(void *buffer, uint32_t size);

/* used to invalidate a block of cache at the given address.                */
void cache_invalidate(void* address,
                      uint32_t size);

/* used to flush a block of cache at the given address.                     */
void cache_flush(void* address,
                 uint32_t size);

/* used to flush and invalidate a block of cache at the given address.      */
void cache_flush_invalidate(void* address,
                            uint32_t size);

/* set context for statistics                                               */
void mem_set_context(void *app_context);

/* used by codec to dynamically allocate memory                             */
void*  mem_alloc(uint32_t size);

/* used by codec to free allocated memory                                   */
void mem_free(void *ptr);

/* debug messages printing support                                          */
void debug_print(const char *format, ...);

#endif //_TEST_HELPER_LIB_H_
