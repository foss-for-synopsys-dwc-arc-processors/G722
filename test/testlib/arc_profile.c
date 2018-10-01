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
#ifdef NATIVE_CYCLE_PROFILING

#include <string.h>
#include "arc_profile.h"
#ifdef _ARC
	#include <arc/arc_timer.h>
	#ifdef NATIVE_BUSBANDWIDTH_PROFILING
		#ifdef _ARC600
			#error "HARDWARE_PROFILING has not been tested on ARC600"
		#endif
		#include "helper_lib.h"
		#ifdef	DCACHE_DISABLED
			#undef NATIVE_BUSBANDWIDTH_PROFILING
		#endif
	#endif
	#ifdef NATIVE_BUSBANDWIDTH_PROFILING
		#include "alb_mss_perfctrl_regmap.h"
	#endif
#endif

static char* component_class_to_str(uint32_t comp_class)
{
    switch (comp_class)
    {
        case ARC_API_CLASS_ENCODER:
            return "ENCODER";
        case ARC_API_CLASS_DECODER:
            return "DECODER";
        case ARC_API_CLASS_TRANSCODER:
            return "TRANSCODER";
        case ARC_API_CLASS_POST_PROCESSOR:
            return "POST-PROCESSOR";
        default:
            return "UNKNOWN";
    }
}

#pragma Push_small_data(0)
extern uint32_t _fstack;
extern uint32_t _estack;
#pragma Pop_small_data()
_Inline static uint32_t GetStackPointer()
{
    return( _core_read(28) );
}

_Inline static void * GetStackStart()
{
    return ((void *) &_fstack);
}

_Inline static void * GetStackEnd()
{
    return ((void *) &_estack);
}

void stack_preprocess(Profiler_Settings_t * inst_settings) {
	uint32_t *stack_start = NULL;
    uint32_t *stack_end = NULL;
	stack_start = (uint32_t *)GetStackStart();     // _fstack is defined in crt1.s: ".weak _fstack; .set _fstack,0"
    stack_end = (uint32_t *)GetStackPointer(); // read Stack Pointer (SP)

    while (stack_start < stack_end) {
        *stack_start++ = 0x7374616B;        // fill stack with markers
    }
	// remember stack usage before frame processing
    inst_settings->profiling_data.stack1 = (uint8_t *)GetStackEnd() - (uint8_t *)GetStackPointer();
}

void stack_postprocess(Profiler_Settings_t * inst_settings) {
	uint32_t *stack_start = NULL;

	stack_start = (uint32_t *)GetStackStart();

    /* Calculate stack usage */
    while (*stack_start++ == 0x7374616B);

    inst_settings->profiling_data.stack2 = (uint8_t *)GetStackEnd() - (uint8_t *)(void*)stack_start;
}


uint32_t print_stream_summary(Profiler_Settings_t * inst_settings)
{
	fprintf(inst_settings->f_raw_profiling_log, "Application: %s" NEWLINE, inst_settings->app_name);
	fprintf(inst_settings->f_raw_profiling_log, "Codec version: @@VERSION_NUMBER@@" NEWLINE);
  #if _ARC
	#ifdef __HIGHC_VER__
	fprintf(inst_settings->f_raw_profiling_log, "Compiler version: %d" NEWLINE, __HIGHC_VER__);
	#endif
	fprintf(inst_settings->f_raw_profiling_log, "ARC version: %d" NEWLINE, _ARCVER);
  #endif
	fprintf(inst_settings->f_raw_profiling_log, "Profiling settings:" NEWLINE);
    fprintf(inst_settings->f_raw_profiling_log, "    Input stream %d:    %s" NEWLINE, 0, inst_settings->stream_config.stream_name);
	fprintf(inst_settings->f_raw_profiling_log, "    Component class:    %s" NEWLINE, component_class_to_str(inst_settings->stream_config.component_class));
	fprintf(inst_settings->f_raw_profiling_log, "    Stream type:        %u" NEWLINE, inst_settings->stream_config.stream_type);
	fprintf(inst_settings->f_raw_profiling_log, "    Samplerate:         %d" NEWLINE, inst_settings->stream_config.sample_rate);
	fprintf(inst_settings->f_raw_profiling_log, "    Bitrate:            %d" NEWLINE, inst_settings->stream_config.bit_rate);
	fprintf(inst_settings->f_raw_profiling_log, "    Number of channels: %d" NEWLINE, inst_settings->stream_config.num_ch);
	fprintf(inst_settings->f_raw_profiling_log, "    Sample size:        %d" NEWLINE, inst_settings->stream_config.sample_size);
	fprintf(inst_settings->f_raw_profiling_log, "    Raw profiling log:  %s" NEWLINE, inst_settings->raw_prof_data_file_name);
	fprintf(inst_settings->f_raw_profiling_log, "    Codec instance size:%u" NEWLINE, inst_settings->stream_config.codec_instance_size);

	fprintf(inst_settings->f_raw_profiling_log, "\nFrame#, bytes, cycles, heap, stack, input buffer, output buffer"
#ifdef NATIVE_BUSBANDWIDTH_PROFILING
		", I$ cycles, D$ cycles"
#endif
	        ", microsec/frame, mhz"
	NEWLINE);

    return TEST_TRUE;
}

void print_frame_info(Profiler_Settings_t * inst_settings)
{
	double mhz = 0;
	uint32_t used_buffer = inst_settings->stream_config.component_class == ARC_API_CLASS_ENCODER ? inst_settings->rt_stats.used_input_buffer : inst_settings->rt_stats.used_output_buffer ;   
	if((inst_settings->profiling_data.mksec_per_frame) > 0)
	mhz = (double)inst_settings->profiling_data.cycles_per_frame /
			(inst_settings->profiling_data.mksec_per_frame);
        fprintf(inst_settings->f_raw_profiling_log, "%u,%u,%u,%u,%lu,%u,%u"
	#ifdef NATIVE_BUSBANDWIDTH_PROFILING
			",%u,%u"
	#endif
			", %u, %.2f"
			NEWLINE , inst_settings->profiling_data.frame_counter,
				used_buffer,
				inst_settings->profiling_data.cycles_per_frame,
				inst_settings->memory_stats.heap_usage,
				inst_settings->profiling_data.stack2 - inst_settings->profiling_data.stack1,
				inst_settings->rt_stats.used_input_buffer,
				inst_settings->rt_stats.used_output_buffer
	#ifdef NATIVE_BUSBANDWIDTH_PROFILING
				, inst_settings->profiling_data.i_cache_cycles_per_frame,
				inst_settings->profiling_data.d_cache_cycles_per_frame
	#endif
				, inst_settings->profiling_data.mksec_per_frame, mhz
				);
        fflush(inst_settings->f_raw_profiling_log);
}

void print_profile_summary(Profiler_Settings_t * inst_settings)
{
	INFO( "[Codec Instance] Codec instance size: %u bytes",
			inst_settings->stream_config.codec_instance_size);
	INFO( "[Stack Usage] Stack usage: %lu bytes",
			inst_settings->profiling_data.stack2 - inst_settings->profiling_data.stack1);
	INFO( "[Heap Usage] Current heap usage (leaks): %d bytes.",
			inst_settings->memory_stats.heap_usage);
	INFO( "[Heap Usage] Maximum heap usage: %d bytes",
			inst_settings->memory_stats.max_heap_usage);
	INFO( "[Memory Usage For Buffers] Maximum I/O buffers memory usage: %d bytes",
			inst_settings->profiling_data.max_used_input_buffer + inst_settings->profiling_data.max_used_output_buffer);
}

#ifdef NATIVE_BUSBANDWIDTH_PROFILING
_Inline void stop_profiler()
{
#if !defined(DCACHE_DISABLED)
    _Uncached int * mss_perfctrl_reg_ctrl = (_Uncached int *)MSS_PERFCTRL_REG_CTRL;
    *mss_perfctrl_reg_ctrl = 0;
#endif
}

_Inline void reset_profiler(int lat)
{
#if !defined(DCACHE_DISABLED)
    _Uncached int * mss_perfctrl_reg_ctrl = (_Uncached int *)MSS_PERFCTRL_REG_CTRL;
    _Uncached int * mss_perfctrl_reg_lat_ctrl = (_Uncached int *)(MSS_PERFCTRL_REG_LAT_CTRL(MSS_PERFCTRL_MEM_IDX));
    *mss_perfctrl_reg_ctrl = 4;
    _nop();
    _nop();
    _nop();
    *mss_perfctrl_reg_ctrl = 1;
    *mss_perfctrl_reg_lat_ctrl = lat;
#endif
}

void read_profiler_counters(uint32_t * read_icache, uint32_t * write_icache, uint32_t * read_dcache, uint32_t * write_dcache)
{
#if !defined(DCACHE_DISABLED)
    *read_icache = *((uint32_t *)MSS_PERFCTRL_REG_R_COUNT_LO(MSS_PERFCTRL_MST_FAB_IBU_AHB_HIDX));
	*write_icache = *((uint32_t *)MSS_PERFCTRL_REG_W_COUNT_LO(MSS_PERFCTRL_MST_FAB_IBU_AHB_HIDX));
	*read_dcache = *((uint32_t *)MSS_PERFCTRL_REG_R_COUNT_LO(MSS_PERFCTRL_MST_FAB_DBU_AHB_HIDX));
	*write_dcache = *((uint32_t *)MSS_PERFCTRL_REG_W_COUNT_LO(MSS_PERFCTRL_MST_FAB_DBU_AHB_HIDX));
#endif
}
#endif


uint32_t read_timer_()
{
#ifdef _ARC
	if (_timer0_present())
	{
		return _timer0_read();
	}
	else if (_timer1_present())
	{
		return _timer1_read();
	}
	else
	{
		WRN0( "No available timers for profiling!");
		return 0;
	}
#else
	return 0;
#endif
}

void reset_timer_()
{
#ifdef _ARC
	if (_timer0_present())
	{
		return _timer0_reset();
	}
	else if (_timer1_present())
	{
		return _timer1_reset();
	}
#endif
}

uint32_t profile_preprocess(Profiler_Settings_t * inst_settings)
{
    memset(&inst_settings->stream_config, 0, sizeof(inst_settings->stream_config));
    memset(&inst_settings->rt_stats, 0, sizeof(inst_settings->rt_stats));

    inst_settings->profiling_data.frame_counter = 0;

    inst_settings->profiling_data.max_used_input_buffer = 0;
    inst_settings->profiling_data.max_used_output_buffer = 0;

    if (strlen(inst_settings->raw_prof_data_file_name) == 0)
    {
        WRN0( "Profiling log file name is not set, use -raw_profiling_data <filename>\n");
        return TEST_FALSE;
    }
	inst_settings->f_raw_profiling_log = fopen(inst_settings->raw_prof_data_file_name, "w");
	if (inst_settings->f_raw_profiling_log == NULL)
    {
        WRN( "Cannot open profiling log file <%s> not set", inst_settings->raw_prof_data_file_name);
        return TEST_FALSE;
    }
    #if defined(NATIVE_BUSBANDWIDTH_PROFILING) 
		inst_settings->profiling_data.i_cache_bus_bw = 0;
		inst_settings->profiling_data.d_cache_bus_bw = 0;
		inst_settings->profiling_data.xydma_bus_bw = 0;
		inst_settings->profiling_data.ldst_bus_bw = 0;
        inst_settings->profiling_data.i_cache_bw_summary = 0;
        inst_settings->profiling_data.d_cache_bw_summary = 0;
        inst_settings->profiling_data.xydma_bw_summary = 0;
        inst_settings->profiling_data.ldst_bw_summary = 0;
    #endif
    return TEST_TRUE;
}

static char* remove_argv_item(int index, int * pargc, char * argv[])
{
    int i=0;
    char* pitem=argv[index];
    int argc=*pargc;
    argc--;
    for(i=index;i<argc;i++)
    {
        argv[i]=argv[i+1];
    }
    argv[argc]=NULL;
    *pargc=argc;
    return pitem;
}

// profile_init function details:
// - it is supposed that this function is called before parsing argc/argv by application
// - this function removes profiling-specific command line options and updates argc/argv values, so application should not doubt about it
// - so, profiling-specific parameters might be located at any position in the command line
void profile_init(Profiler_Settings_t *inst_settings, int * pargc, char * argv[])
{
    uint32_t count = 1;
    uint32_t prof_counters = 0;
    char* argv_item = NULL;
    inst_settings->profiling_data.nframes = 0;

    INFO("%s", "===");
    INFO("%s", "Profiler specific command line parameters");
    INFO("%s", "---");
    while(count < (uint32_t)(*pargc))
    {
        if(strcmp(argv[count], "-raw_profiling_data")==0)
        {
            remove_argv_item(count, pargc, argv); /* remove -raw_profiling_data */
            argv_item = remove_argv_item(count, pargc, argv);
            memcpy(inst_settings->raw_prof_data_file_name, argv_item, strlen(argv_item)+1);
            INFO("Profiling data file = %s", inst_settings->raw_prof_data_file_name);
            prof_counters+=2;
        }
        else if(strcmp(argv[count], "-memory_latency")==0)
        {
            remove_argv_item(count, pargc, argv); /* remove -memory_latency */
            argv_item = remove_argv_item(count, pargc, argv);
#ifdef NATIVE_BUSBANDWIDTH_PROFILING
            inst_settings->profiling_data.required_mem_latency = atoi(argv_item);
            INFO("required_mem_latency = %u", inst_settings->profiling_data.required_mem_latency);
#endif
            prof_counters+=2;
        }
        else if(strcmp(argv[count], "-nframes")==0)
        {
            remove_argv_item(count, pargc, argv); /* remove -memory_latency */
            argv_item = remove_argv_item(count, pargc, argv);
            inst_settings->profiling_data.nframes = atoi(argv_item);
            INFO("nframes = %u", inst_settings->profiling_data.nframes);
            prof_counters+=2;
        }
        else count++;
    }
    INFO("%s", "===");
}

TEST_BOOL profile_frame_preprocess(Profiler_Settings_t * inst_settings)
{
    if (inst_settings->profiling_data.frame_counter >= inst_settings->profiling_data.nframes && inst_settings->profiling_data.nframes > 0)
    {
    	DBG("frame_counter=%u",inst_settings->profiling_data.frame_counter);
        return TEST_FALSE;
    }
#ifdef NATIVE_STACK_PROFILING
    stack_preprocess(inst_settings);
#endif
#ifdef NATIVE_BUSBANDWIDTH_PROFILING
	{
		int x = inst_settings->profiling_data.required_mem_latency;
		reset_profiler(x);
	}
#endif
    reset_timer_();
    inst_settings->profiling_data.c1 = read_timer_();

    return TEST_TRUE;
}

void store_frame_info(Profiler_Settings_t * inst_settings)
{
#ifdef NATIVE_BUSBANDWIDTH_PROFILING
	uint32_t read_icache, read_dcache, write_icache, write_dcache;
	read_profiler_counters(&read_icache, &write_icache, &read_dcache, &write_dcache);

    inst_settings->profiling_data.i_cache_cycles_per_frame = read_icache + write_icache;
    inst_settings->profiling_data.d_cache_cycles_per_frame = read_dcache + write_dcache;
    inst_settings->profiling_data.i_cache_bw_summary+=inst_settings->profiling_data.i_cache_cycles_per_frame*4;
    inst_settings->profiling_data.d_cache_bw_summary+=inst_settings->profiling_data.d_cache_cycles_per_frame*4;
#endif

    inst_settings->profiling_data.mksec_per_frame = inst_settings->rt_stats.microseconds_per_frame;
    inst_settings->profiling_data.cycles_per_frame = (inst_settings->profiling_data.c2 - inst_settings->profiling_data.c1);
    inst_settings->profiling_data.summary_cycles += inst_settings->profiling_data.cycles_per_frame;
}

TEST_BOOL profile_frame_postprocess(Profiler_Settings_t * inst_settings)
{
    inst_settings->profiling_data.c2 = read_timer_();
#ifdef NATIVE_BUSBANDWIDTH_PROFILING
    stop_profiler();
#endif

    if (inst_settings->profiling_data.frame_counter >= inst_settings->profiling_data.nframes && inst_settings->profiling_data.nframes > 0)
    {
        return TEST_FALSE;
    }
    if(inst_settings->profiling_data.cached_sample_rate != inst_settings->stream_config.sample_rate ||
    		inst_settings->profiling_data.cached_sample_size != inst_settings->stream_config.sample_size  || 
			inst_settings->profiling_data.cached_num_chans != inst_settings->stream_config.num_ch )
    {
    	inst_settings->profiling_data.cached_sample_rate = inst_settings->stream_config.sample_rate;
    	inst_settings->profiling_data.cached_sample_size = inst_settings->stream_config.sample_size;
		inst_settings->profiling_data.cached_num_chans = inst_settings->stream_config.num_ch;
    	print_stream_summary(inst_settings);
    }
#ifdef NATIVE_STACK_PROFILING
    stack_postprocess(inst_settings);
#endif
    store_frame_info(inst_settings);
    print_frame_info(inst_settings);
    inst_settings->profiling_data.max_used_output_buffer = _max(inst_settings->profiling_data.max_used_output_buffer, inst_settings->rt_stats.used_output_buffer);
    inst_settings->profiling_data.max_used_input_buffer = _max(inst_settings->profiling_data.max_used_input_buffer, inst_settings->rt_stats.used_input_buffer);

    memset(&inst_settings->rt_stats, 0, sizeof(inst_settings->rt_stats));
    inst_settings->profiling_data.frame_counter++;
    return TEST_TRUE;
}

void profile_postprocess(Profiler_Settings_t* inst_settings)
{
    print_profile_summary(inst_settings);
    if (inst_settings->f_raw_profiling_log != NULL)
    {
	    fflush(inst_settings->f_raw_profiling_log);
	    fclose(inst_settings->f_raw_profiling_log);
	    inst_settings->f_raw_profiling_log = NULL;
    }
    fflush(stdout);
}

void profile_pause(Profiler_Settings_t * inst_settings)
{
	inst_settings->profiling_data.c_stop = read_timer_();
}

void profile_resume(Profiler_Settings_t * inst_settings)
{
	inst_settings->profiling_data.c1 +=  read_timer_() - inst_settings->profiling_data.c_stop;
}
#endif
