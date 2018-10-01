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

#ifndef _TEST_TYPES_H_
#define _TEST_TYPES_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "testlib_assert.h"

#ifdef _ARC
#if _ARCVER > 0x39 
    #include <arc/arc_reg.h>
    #include <arc/arc_timer.h>
    #ifdef NATIVE_BUSBANDWIDTH_PROFILING
    		#include "helper_lib.h"
		#ifdef	DCACHE_DISABLED
			#undef NATIVE_BUSBANDWIDTH_PROFILING
		#endif
    #endif
#else
    #include "platform_lib/arc.h"
#endif
#endif

#ifdef _MSC_VER
  #define _max(a, b) (a > b) ? a : b
#endif

#ifndef PSP_MEMORY_ALIGNMENT
    #ifdef CACHE_LINE_SIZE
     #define PSP_MEMORY_ALIGNMENT (CACHE_LINE_SIZE-1) /*cache line - 1*/
    #else 
     #define PSP_MEMORY_ALIGNMENT (31) /*cache line - 1*/
    #endif
#endif

#define TEST_VERSION                     "1.0"
#define MAX_FILENAME_LENGTH             (255)   /* not including the terminating '0'                */
#define MAX_NUMBER_OF_FILE_PARAMS       (7)     /* filename + 5 parameters                          */
#define MAX_ARGUMENT_LENGTH             (383)   /* max length of one argument in the command line   */

#ifndef MAX_USER_ARGS 
#define MAX_USER_ARGS                   (20)    /* number of codec-specific command-line parameters */
#endif

#define NEWLINE                         "\r\n"

typedef char TEST_BOOL;
#define TEST_TRUE 0x01
#define TEST_FALSE 0x00

typedef enum
{
    TEST_ERROR,
    TEST_WARNING,
    TEST_INFO
} Test_Message_Type_t;

#define INPUT_BUFFER_SIZE_DETECT    8192 

#ifndef DEFAULT_MAX_NUMBER_OF_CHANNELS
#define DEFAULT_MAX_NUMBER_OF_CHANNELS  10
#endif

typedef enum {
    ARC_API_IO_ID_INPUT = 1,     /* Input buffers data */
    ARC_API_IO_ID_OUTPUT         /* Output buffers data */
} ARC_API_IO_ID_t;

typedef struct {
    uint32_t heap_usage;                /* Current heap usage           */
    uint32_t max_heap_usage;            /* Max heap usage               */
} ARC_API_Memory_Context_t;

typedef enum {
    OUTPUT_MODE_SAVE_NOTHING        = 0x00000000,
    OUTPUT_MODE_SAVE_DATA           = 0x00000001,
    OUTPUT_MODE_SAVE_MD5            = 0x00000002,
    OUTPUT_MODE_SAVE_PARTIAL_MD5    = 0x00000004,
    OUTPUT_MODE_SAVE_PARTIAL_CRC32  = 0x00000008
} Test_Output_Mode_t;

typedef enum {
    ARC_API_CLASS_ENCODER = 1,                /* 1 - encoder                      */
    ARC_API_CLASS_DECODER = 2,                /* 2 - decoder                      */
    ARC_API_CLASS_TRANSCODER = 3,             /* 3 - transcoder                   */
    ARC_API_CLASS_POST_PROCESSOR = 4          /* 4 - post-processing component    */
} ARC_API_Component_Class_t;

typedef enum {
    STREAM_TYPE_MPEG_AUDIO_VERSION_1_LAYER_1 = 1,
    STREAM_TYPE_MPEG_AUDIO_VERSION_1_LAYER_2,
    STREAM_TYPE_MPEG_AUDIO_VERSION_1_LAYER_3,
    STREAM_TYPE_MPEG_AUDIO_VERSION_2_LAYER_1,
    STREAM_TYPE_MPEG_AUDIO_VERSION_2_LAYER_2,
    STREAM_TYPE_MPEG_AUDIO_VERSION_2_LAYER_3,
    STREAM_TYPE_MPEG_AUDIO_VERSION_2_5_LAYER_1,
    STREAM_TYPE_MPEG_AUDIO_VERSION_2_5_LAYER_2,
    STREAM_TYPE_MPEG_AUDIO_VERSION_2_5_LAYER_3,
    STREAM_TYPE_AMRWB,
    STREAM_TYPE_AMRWB_PLUS,
    STREAM_TYPE_AAC,
    STREAM_TYPE_WMA,
    STREAM_TYPE_PCM_INTERLEAVED, /* Little-endian PCM                       */
    STREAM_TYPE_PCM_INTERLEAVED_LE = STREAM_TYPE_PCM_INTERLEAVED,   /* alias*/
    STREAM_TYPE_PCM_NONINTERLEAVED,
    STREAM_TYPE_SERVICE_DATA,
    STREAM_TYPE_LCSBC,          /* Bluetooth low-complexity, subband codec  */
    STREAM_TYPE_AC3,            /* Dolby Digital - AC3                      */
    STREAM_TYPE_EC3,            /* Dolby Digital Plus - Enhanced AC3        */
    STREAM_TYPE_DTS,            /* DTS-HD Master Audio encoded stream       */
    STREAM_TYPE_AMRNB,
    STREAM_TYPE_AMRNB_RFC3267,
    STREAM_TYPE_MSF_IO_CONTROL_DATA, /* For MSF to control audio components */
    STREAM_TYPE_DTSLBR,              /* DTS Express                         */
    STREAM_TYPE_DOLBY_TRUEHD,        /* Dolby TrueHD encoded stream         */
    STREAM_TYPE_BLURAY_DRA,     /* Dynamic Resolution Adaptation. It is one */
                                /* of the standard formats for Blu-Ray      */
                                /* audio, introduced with Blu-Ray 2.3.      */
    STREAM_TYPE_PCM_INTERLEAVED_BE,  /* Big-endian PCM                      */
    STREAM_TYPE_VORBIS,         /* VORBIS audio                             */
    STREAM_TYPE_G726,
    STREAM_TYPE_G711_ALAW_PCM,
    STREAM_TYPE_G711_uLAW_PCM,
    STREAM_TYPE_APE,            /* APE Audio                                */
    STREAM_TYPE_FLAC,           /* FLAC Audio (http://flac.sourceforge.net) */
    STREAM_TYPE_ALAC,           /* ALAC Audio                               */

    STREAM_TYPE_MPEG_AUDIO_LAYER_1,
    STREAM_TYPE_MPEG_AUDIO_LAYER_2,
    STREAM_TYPE_MPEG_AUDIO_LAYER_3,

    STREAM_TYPE_OPUS,

    STREAM_TYPE_LAST
} ARC_API_Stream_Type_t;

typedef enum {
    /* No errors. Successful function completion */
    ARC_API_ERR_NONE = 0,

    /* Memory or buffer allocation error or passed memory block does not have enough
       room to receive data from codec. Application may need to adjust heap manager
       parameters or reallocate the buffer passed to codec (see call description for
       details). */
    ARC_API_ERR_NOT_ENOUGH_MEMORY,

    /* Parameter ID, control command ID is valid but not supported by a given codec instance */
    ARC_API_ERR_NOT_SUPPORTED,

    /* Parameter value passed to the codec is not valid */
    ARC_API_ERR_INVALID_PARAMETER_VALUE,

    /* Input data cannot be processed */
    ARC_API_ERR_INVALID_INPUT_DATA,

    /* Codec has finished accumulated data flushing, it can be properly resetted to
       start new stream processing */
    ARC_API_ERR_DATA_FLUSH_DONE,

    /* Application need to check all input buffers state flags to define the type of
       requested operation and perform this operation. */
    ARC_API_ERR_CHECK_INPUT_BUFFER,

    /* Application need to check all output buffers state flags to define the type of
       requested operation and perform this operation. */
    ARC_API_ERR_CHECK_OUTPUT_BUFFER,

    /* Data processing attempt cannot be done until demultiplexer-specific data is not set */
    ARC_API_ERR_NEEDS_DEMUXER_DATA

} ARC_API_Error_Code_t;

typedef struct {
/*  Audio stream type (format): MP3, AAC, WMA, etc...
    See stream types definition in the arc_audio_api_stream_types.h file */
	ARC_API_Stream_Type_t stream_type;
/* encoder/decoder/transcoder etc       */
	ARC_API_Component_Class_t               component_class;
/*  Sample Rate of PCM data (Hz) */
    uint32_t sample_rate;
/*  Sample Size of PCM data (bytes) */
    uint32_t sample_size;
/*  Bitrate */
    uint32_t bit_rate;
/*  Number of PCM data channels which is currently present in buffer */
    uint32_t num_ch;
/* entire size of the Codec instance */
    uint32_t codec_instance_size;
    char*    stream_name;
} ARC_API_Stream_Configuration_t;

typedef struct {
    unsigned long c1;
    unsigned long c2;
    unsigned long c_stop;
 	unsigned long stack1;
 	unsigned long stack2;

    uint32_t nframes;
    uint32_t frame_counter;       /* Number of frames per profiling interval. Old scheme  */

    uint32_t mksec_per_frame;
    uint32_t cycles_per_frame;
    uint32_t summary_cycles;
    uint32_t cached_sample_rate;
    uint32_t cached_sample_size;
	uint32_t cached_num_chans;
   
    uint32_t max_used_input_buffer;
    uint32_t max_used_output_buffer;

#ifdef NATIVE_BUSBANDWIDTH_PROFILING
	double i_cache_bus_bw;    //I$
	double d_cache_bus_bw;    //D$
	double xydma_bus_bw;    //XY DMA
	double ldst_bus_bw;     //LD/ST
    //arrays for storing BWs for every bus
    uint32_t i_cache_cycles_per_frame;
    uint32_t d_cache_cycles_per_frame;
    //BWs sums for moving average calculation
    double    i_cache_bw_summary;
    double    d_cache_bw_summary;
    //arrays for storing BWs for every bus
    uint32_t xydma_cycles_per_pf_frame;
    int32_t  ldst_cycles_per_pf_frame;
    //BWs sums for moving average calculation
    double    xydma_bw_summary;
    double    ldst_bw_summary;
    uint32_t  required_mem_latency;
#endif
} ARC_TEST_Profile_Tag_t;

typedef struct {
	uint32_t microseconds_per_frame;
	uint32_t used_input_buffer;
	uint32_t used_output_buffer;
} ARC_API_Real_Time_Statistics_t;

typedef struct
{
	char                                    app_name[MAX_FILENAME_LENGTH];
	char                                    input_file_name[MAX_FILENAME_LENGTH];
	char                                    output_file_name[MAX_FILENAME_LENGTH];
	char 									*list;
	FILE 									*fout, *fin;
	uint32_t 								error_occurred;


    uint32_t                                bb_first_broken_bit_offset;
    uint32_t                                bb_distance_to_next_broken_bit;
	uint32_t                                bb_brokenByte;

	ARC_API_Memory_Context_t                memory_stats;
    uint32_t                                malloc_counter;
    uint32_t                                stress_test_counter;
    uint32_t                                stress_test_iter_counter;
    uint32_t                                f_stress_test_enabled;

	char                                    raw_prof_data_file_name[MAX_FILENAME_LENGTH];
	FILE *                                  f_raw_profiling_log;
	ARC_TEST_Profile_Tag_t 					profiling_data;
	ARC_API_Stream_Configuration_t			stream_config;
	ARC_API_Real_Time_Statistics_t			rt_stats;
    uint32_t                                flag_ext_wav_header;
} Application_Settings_t;


#define DBG0(...) //printf("[DEBUG] %s:%d\n" , __FUNCTION__, __LINE__);
#define DBG(fmt, ...) //printf("[DEBUG] %s:%d:" fmt "\n",  __FUNCTION__, __LINE__, __VA_ARGS__);
#define WRN0(fmt, ...) printf("[WARNING] " fmt "\n");
#define WRN(fmt, ...) printf("[WARNING] " fmt "\n",  __VA_ARGS__);
#define INFO(fmt, ...) printf("[INFO] " fmt "\n",  __VA_ARGS__);
#define ERR(fmt, ...) fprintf(stderr, "[ERROR] " fmt "\n");

#endif // _TEST_TYPES_H_
