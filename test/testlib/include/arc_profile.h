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

#ifndef __TEST_PROFILE_H__
#define __TEST_PROFILE_H__

#ifdef NATIVE_CYCLE_PROFILING
#include <stdint.h>
#include "testlib_types.h"

typedef Application_Settings_t Profiler_Settings_t;

void profile_init(Profiler_Settings_t *inst_settings, int * pargc, char * argv[]);
TEST_BOOL profile_frame_preprocess(Profiler_Settings_t * inst_settings);
TEST_BOOL profile_frame_postprocess(Profiler_Settings_t * inst_settings);
uint32_t profile_preprocess(Profiler_Settings_t * inst_settings);
void profile_postprocess(Profiler_Settings_t * inst_settings);
void profile_pause(Profiler_Settings_t * inst_settings);
void profile_resume(Profiler_Settings_t * inst_settings);
#endif

#endif //__TEST_PROFILE_H__
