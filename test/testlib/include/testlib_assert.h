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
// ARC Product:  ARC Audio API 1.0
// File version: $Revision$
//
// Description:
//     Custom ASSERT macro definition.
//     Aimed to be used in codec source code only. Do not use
//     in application source code since it requires
//     Helper Library call which is not available for 
//     application.
//
//
// Author:
//
*/

#ifndef _TEST_ASSERT_H_
#define _TEST_ASSERT_H_

#ifdef _ARC
#ifndef TEST_ASSERT_DISABLED
#define TEST_ASSERT(cond)   { if(!(cond)) { DBG("TEST ASSERTION FAILED on %s:%d: %s\n", __FILE__, __LINE__, #cond); _brk(); } }
#else
#define TEST_ASSERT(cond)
#endif
#else
#include <assert.h>
#define TEST_ASSERT assert
#endif
#endif //TEST_ASSERT_H_
