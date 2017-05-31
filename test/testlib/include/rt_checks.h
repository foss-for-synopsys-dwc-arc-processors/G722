#include "testlib_types.h"

#ifndef __RT_CHEKS_H__
#define __RT_CHEKS_H__

#if (defined NATIVE_MUL_AUX_REGS_TEST) || (defined NATIVE_XY_GARBAGE_TEST) || (defined NATIVE_BROKEN_BIT_TEST) \
    || (defined NATIVE_DCACHE_TEST) || (defined NATIVE_BROKEN_BIT_TEST) || (defined NATIVE_TRICK_PLAY) \
    || (defined NATIVE_STRESS_TEST)  || (defined NATIVE_INTERRUPT_TEST)
	#define TESTLIB_ENABLED
	void TESTLIB_init(int* argc, char * argv[]);
	TEST_BOOL TESTLIB_frame_preprocess(char *input, uint32_t len);
	TEST_BOOL TESTLIB_frame_postprocess(char *output, uint32_t len);
	void TESTLIB_done();
	
	extern Application_Settings_t app_settings;
#endif

#endif
