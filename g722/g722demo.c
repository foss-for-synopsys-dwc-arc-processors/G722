/*                     v3.0 - 10/Jan/2007
  ============================================================================

  G722DEMO.C 
  ~~~~~~~~~

  Description: 
  ~~~~~~~~~~~~
  
  Demonstration program for UGST/ITU-T G.722 module with the G.722 
  decoding function.

  Input data is supposed to be aligned at word boundaries, i.e.,
  organized in 16-bit words, following the operating system normal
  organization (low-byte first for VMS and DOS; high byte first for most
  Unix systems). Input linear samples are supposed to be 16-bit right-adjusted
  and the 7kHz ADPCM bitstream is left-adjusted, i.e., the codewords are 
  located in the lower 8-bits of the encoded bitstream file. The MSB is
  always 0 for the bitstream file.
  
  Usage:
  ~~~~~~
  $ G722DEMO [-options] InpFile OutFile 
             [Mode [[BlockSize [1stBlock [NoOfBlocks]]]]]
  where:
  InpFile     is the name of the file to be processed;
  OutFile     is the name with the processed data;
  Mode        is the operation mode for the G.722 decoder (1,2, or 3). 
  BlockSize   is the block size, in number of samples [default: 1024 samples]
  1stBlock    is the number of the first block of the input file
              to be processed [default: 1st block]
  NoOfBlocks  is the number of blocks to be processed, starting on
    	      block "1stBlock" [default: all blocks]

  Options:
  -mode #     is the operation mode for the G.722 decoder. 
              Default is 64 kbit/s.
  -frame #    Number of samples per frame for switching bit rates.
              Default is 16 samples (or 2ms) 
  -enc        run only the encoder [default: encoder and decoder]
  -dec        run only the decoder [default: encoder and decoder]
  -noreset    don't apply reset to the encoder/decoder
  -q          quiet operation (don't print progress flag)
  -?/-help    print help message

  Original author:
  ~~~~~~~~~~~~~~~~
  Simao Ferraz de Campos Neto
  Comsat Laboratories                  Tel:    +1-301-428-4516
  22300 Comsat Drive                   Fax:    +1-301-428-9287
  Clarksburg MD 20871 - USA            E-mail: simao@ctd.comsat.com
    
  History:
  ~~~~~~~~
  01.Jul.95    v1.0    Created based on CNET's encoder and decode demo 
                       programs version 2.0 ({enc,dec}g722.c).
  10.Aug.95    v2.0    Fixed memory invasion bug; insufficient memory 
                       allocated for the output buffer
  13.Sep.96    v2.1    Fixed display_usage() <simao>
  04.Jan.99    v2.2    Fixed bug in the claculation of the number of 
                       blocks to process. Earlier version was clipping
                       the last samples in the input file if the file
                       size was not a multiple of the block size
                       N. <simao>
  10.Jan.07    v3.0    Added some castings to avoid warnings
  ============================================================================
*/

/* Standard prototypes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <math.h>

#ifdef VMS
#include <stat.h>
#else
#include <sys/stat.h>
#endif

/* G.722- and UGST-specific prototypes */
#include "g722.h"
#include "ugstdemo.h"

/* Local prototypes */
void display_usage ARGS((void));

/* Local definitions */
#define DFT_BLK 1024

#ifdef _ARC
//#include <arc/arc_intrinsics.h>
#include <arc/arc_timer.h>
#include <stdint.h>
#include "arc_profile.h"
#include "helper_lib.h"
#include "rt_checks.h"
#endif

#ifdef NATIVE_CYCLE_PROFILING
#include "helper_lib.h"
#endif
/*
 -------------------------------------------------------------------------
 void display_usage(void);
 ~~~~~~~~~~~~~~~~~~
 Display proper usage for the demo program. Generated automatically from
 program documentation.

 History:
 ~~~~~~~~
 01.Jul.95 v1.0 Created <simao>.
 -------------------------------------------------------------------------
*/
#define P(x) printf x
void display_usage()
{
  /* Print Message */
  P(("G722DEMO Version 2.2 of 04/Jan/1999 \n"));
  P(("  UGST/ITU-T G.722 wideband (50-7000Hz) encode/decode module.\n"));
  P(("  (*) G.722 Module: COPYRIGHT CNET LANNION A TSS/CMC, 24/Aug/90\n"));

  P(("  Usage:\n"));
  P(("  $ G722DEMO [-options] InpFile OutFile Mode\n"));
  P(("             [BlockSize [1stBlock [NoOfBlocks]]]\n"));
  P(("  where:\n"));
  P(("  %s%s\n", "InpFile     ",
                 "name of file to be processed (16 bit, left justified)"));
  P(("  OutFile     filename for processed data\n"));
  P(("  Mode        operation mode for the G.722 decoder (1,2,or 3)\n"));
  P(("              Default is 1, i.e., 64 kbit/s.\n"));
  P(("  BlockSize   block size, in number of samples [default: 1024 samples]\n"));
  P(("  1stBlock    first block in input file to be processed [default: 1st block]\n"));
  P(("  NoOfBlocks  number of blocks to be processed, starting on\n"));
  P(("    	    block \"1stBlock\" [default: all blocks]\n"));

  P(("  Options:\n"));
  P(("  -mode #     operation mode for the G.722 decoder (1[default],2,or 3). \n"));
  P(("  -frame #    Number of samples per frame [default: 1024]\n"));
  P(("  -enc        run only the encoder [default: encoder and decoder]\n"));
  P(("  -dec        run only the decoder [default: encoder and decoder]\n"));
  P(("  -noreset    don't apply reset to the encoder/decoder\n"));
  P(("  -?/-help    print help message\n"));
  P(("  -q          quiet operation (don't print progress flag)\n"));

  /* Quit program */
  exit(-128);
}
#undef P
/* .................... End of display_usage() ........................... */

#if defined (NATIVE_CYCLE_PROFILING) 
Application_Settings_t app_settings = {0};
#endif

/*
   **************************************************************************
   ***                                                                    ***
   ***        Demo-Program for testing the correct implementation         ***
   ***               and to show how to use the programs                  ***
   ***                                                                    ***
   **************************************************************************
*/
int 
main (argc, argv)
int argc;
char *argv[];
{

  /* Codec related declarations */
  Word16          mode = 1;
  Word16          reset;
  g722_state      encoder, decoder;

  /* Encode and decode operation specification: both as default */
  char encode=1, decode=1;

  /* Sample buffers */
#ifdef STATIC_ALLOCATION
  Word16   code[DFT_BLK];
  Word16   incode[DFT_BLK], outcode[DFT_BLK];
#else
  Word16   *code=NULL; /* Bitstream buffer */
  Word16   *incode=NULL, *outcode=NULL; /*Input and output buffers */
#endif
  Word16   *inp_buf, *cod_buf, *out_buf;

  /* File variables */
  char            FileIn[256], FileOut[256];
  FILE           *inp, *out;
  int             read1;
  long            iter=0;
  long            N=DFT_BLK, N1=1, N2=0, smpno=0;
  long            start_byte;
#ifdef VMS
  char            mrs[15];
#endif

  /* Progress flag indicator */
  static char     quiet=0, funny[9] = "|/-\\|/-\\";

#ifdef NATIVE_CYCLE_PROFILING
  mem_set_context(&app_settings);
  profile_init(&app_settings, &argc, argv);
#endif

  /* *** ......... PARAMETERS FOR PROCESSING ......... *** */

  /* GETTING OPTIONS */

  if (argc < 2)
    display_usage();
  else
  {
    while (argc > 1 && argv[1][0] == '-')
      if (strcmp(argv[1], "-noreset") == 0)
      {
	/* No reset */
	reset = 0;

	/* Update argc/argv to next valid option/argument */
	argv++;
	argc--;
      }
      else if (strcmp(argv[1], "-mode") == 0)
      {
	/* Define decoder operation mode */
        mode = atoi(argv[2]);

	/* Protect mode, if misgiven */
	if (mode < 1 || mode > 3)
	  HARAKIRI("Bad mode specified; aborting\n", 2);

	/* Move argv over the option to the next argument */
	argv+=2;
	argc-=2;
      }
      else if (strcmp(argv[1], "-enc") == 0)
      {
	/* Encoder-only operation */
	encode = 1;
	decode = 0;

	/* Move argv over the option to the next argument */
	argv++;
	argc--;
      }
      else if (strcmp(argv[1], "-dec") == 0)
      {
	/*Decoder-only operation */
	encode = 0;
	decode = 1;

	/* Move argv over the option to the next argument */
	argv++;
	argc--;
      }
      else if (strcmp(argv[1], "-frame") == 0)
      {
	/* Define Frame size for rate change during operation */
        N = atoi(argv[2]);

	/* Move argv over the option to the next argument */
	argv+=2;
	argc-=2;
      }
      else if (strcmp(argv[1], "-q") == 0)
      {
	/* Don't print progress indicator */
	quiet = 1;

	/* Move argv over the option to the next argument */
	argv++;
	argc--;
      }
      else if (strcmp(argv[1], "-?") == 0 || strstr(argv[1], "-help"))
      {
	/* Print help */
	display_usage();
      }
      else
      {
	fprintf(stderr, "ERROR! Invalid option \"%s\" in command line\n\n",
		argv[1]);
	display_usage();
      }
  }

  /* Now get regular parameters */
  GET_PAR_S(1, "_Input File: .................. ", FileIn);
  GET_PAR_S(2, "_Output File: ................. ", FileOut);
  FIND_PAR_I(3, "_Mode: ........................ ", mode, mode);
  FIND_PAR_L(4, "_Block Size: .................. ", N, N);
  FIND_PAR_L(5, "_Starting Block: .............. ", N1, N1);
  FIND_PAR_L(6, "_No. of Blocks: ............... ", N2, N2);

  /* Find starting byte in file */
  start_byte = sizeof(short) * (long) (--N1) * (long) N;

  /* Check if is to process the whole file */
  //if (N2 == 0)
  //{
  //  struct stat     st;

  //  /* ... find the input file size ... */
  //  stat(FileIn, &st);
  //  N2 = (long)ceil((st.st_size - start_byte) / (double)(N * sizeof(short)));
  //}

  /* Protect mode, if misgiven */
  if (mode < 1 || mode > 3)
    HARAKIRI("Bad mode specified; aborting\n", 2);

  /* Open input file */
  if ((inp = fopen (FileIn, RB)) == NULL)
    KILL(FileIn, -2);

  /* Open output file */
  if ((out = fopen (FileOut, WB)) == NULL)
    KILL(FileOut, -2);

#ifndef STATIC_ALLOCATION
#ifndef NATIVE_CYCLE_PROFILING
  /* Allocate necessary memory and initialize pointers */
  if (encode && decode)
  {
    inp_buf = incode = (Word16 *) calloc(N, sizeof(Word16));
    out_buf = outcode = (Word16 *) calloc(N, sizeof(Word16));
    cod_buf = code = (Word16 *) calloc(N/2, sizeof(Word16));
    if (inp_buf==NULL) HARAKIRI("Error alocating input buffer\n",3);
    if (out_buf==NULL) HARAKIRI("Error alocating output buffer\n",3);
    if (cod_buf==NULL) HARAKIRI("Error alocating bitstream buffer\n",3);
  }
  else if (encode)
  {
    /* It is a encode-only operation */
    inp_buf = incode = (Word16 *) calloc(N, sizeof(Word16));
    out_buf = cod_buf = code = (Word16 *) calloc(N/2, sizeof(Word16));
    if (inp_buf==NULL) HARAKIRI("Error alocating input buffer\n",3);
    if (cod_buf==NULL) HARAKIRI("Error alocating bitstream buffer\n",3);
  }
  else
  {
    /* It is a decode-only operation */
    inp_buf = cod_buf = incode = (Word16 *) calloc(N, sizeof(Word16));
    outcode = out_buf = (Word16 *) calloc(2l*N, sizeof(Word16));
    if (cod_buf==NULL) HARAKIRI("Error alocating bitstream buffer\n",3);
    if (out_buf==NULL) HARAKIRI("Error alocating output buffer\n",3);
  }
#else
  /* Allocate necessary memory and initialize pointers */
  if (encode && decode)
  {
    inp_buf = incode = (Word16 *) mem_alloc(N * sizeof(Word16));
    memset(inp_buf, 0, N * sizeof(Word16) );
    out_buf = outcode = (Word16 *) mem_alloc(N * sizeof(Word16));
    memset(out_buf, 0, N * sizeof(Word16) );
    cod_buf = code = (Word16 *) mem_alloc(N/2 * sizeof(Word16));
    memset(cod_buf, 0, N/2 * sizeof(Word16) );
    if (inp_buf==NULL) HARAKIRI("Error alocating input buffer\n",3);
    if (out_buf==NULL) HARAKIRI("Error alocating output buffer\n",3);
    if (cod_buf==NULL) HARAKIRI("Error alocating bitstream buffer\n",3);
  }
  else if (encode)
  {
    /* It is a encode-only operation */
    inp_buf = incode = (Word16 *) mem_alloc(N * sizeof(Word16));
    memset(inp_buf, 0, N * sizeof(Word16) );
    out_buf = cod_buf = code = (Word16 *) mem_alloc(N/2 * sizeof(Word16));
    memset(out_buf, 0, N/2 * sizeof(Word16) );
    if (inp_buf==NULL) HARAKIRI("Error alocating input buffer\n",3);
    if (cod_buf==NULL) HARAKIRI("Error alocating bitstream buffer\n",3);
  }
  else
  {
    /* It is a decode-only operation */
    inp_buf = cod_buf = incode = (Word16 *) mem_alloc(N * sizeof(Word16));
    memset(inp_buf, 0, N * sizeof(Word16) );
    outcode = out_buf = (Word16 *) mem_alloc(2l*N * sizeof(Word16));
    memset(out_buf, 0, 2l*N * sizeof(Word16) );
    if (cod_buf==NULL) HARAKIRI("Error alocating bitstream buffer\n",3);
    if (out_buf==NULL) HARAKIRI("Error alocating output buffer\n",3);
  }

#endif
#endif

  /* Reset lower and upper band encoders and define input/output buffers */
#ifdef STATIC_ALLOCATION
  if (encode)
  {
    g722_reset_encoder(&encoder);
    cod_buf = code;
  }
  else
    cod_buf = incode;

  inp_buf = incode;

  if (decode)
  {
    g722_reset_decoder(&decoder);
    out_buf = outcode;
  }
  else
    out_buf = code;
#else
  if (encode)
    g722_reset_encoder(&encoder);
  if (decode)
    g722_reset_decoder(&decoder);
#endif

  /* Adjust number of samples if decode only and frame size too big */
  if (decode && !encode && N>8192)
    N/=2;

  /* *** Read samples from input file and decode ***
     NOTE: Number of output samples:
           - if encoder + decoder, #inp samples = #out samples
	   - if encoder only, #out samples = half of # of input samples
	   - if decoder only, #out samples = double the # of input 
	     bitstream samples
     ***
   */
#ifdef NATIVE_CYCLE_PROFILING
  profile_preprocess(&app_settings);
   if ((decode)&&(encode))
   {
       printf("\n ERROR: Profiling can used only for encoder or only for decoder. Use key -enc or -dec in command line for choose one from them \n");
       exit(1);
   }

  
  

#endif
  while ((read1 = fread(incode, sizeof (short), N, inp)) != 0)
  {
    /* print progress flag */
    if (!quiet)
      fprintf(stderr, "%c\r", funny[(iter/read1) % 8]);

#ifdef PROFILING_CODEC_SAMPLES
        _timer0_reset();
#endif

    if (encode)
    {
#ifdef PROFILING_CODEC_SAMPLES
        static uint32_t print_header_encoder=1;
        uint32_t timer0;
        timer0=_timer0_read();
#endif

#ifdef NATIVE_CYCLE_PROFILING
        
            unsigned bytes_ps = 2;
            unsigned sample_rate = 16000;
            unsigned channels=1;
            unsigned blocksize=read1;
            unsigned size = blocksize*(bytes_ps)*channels;
            if(sample_rate != app_settings.stream_config.sample_rate || bytes_ps != app_settings.stream_config.sample_size)
            {
                app_settings.stream_config.stream_name = FileIn;
                app_settings.stream_config.stream_type = 0;
                app_settings.stream_config.component_class = ARC_API_CLASS_ENCODER;
                app_settings.stream_config.sample_rate =sample_rate;
                app_settings.stream_config.sample_size = bytes_ps;
                app_settings.stream_config.bit_rate = 0;
                app_settings.stream_config.num_ch =channels;
                app_settings.stream_config.codec_instance_size = sizeof(g722_state )*2;
            }
            app_settings.rt_stats.microseconds_per_frame = (unsigned)1000000*blocksize/sample_rate;
            app_settings.rt_stats.used_input_buffer = size;
        
        profile_frame_preprocess(&app_settings);
#endif

      /* Encode */
      smpno = g722_encode(inp_buf, code, read1, &encoder);
      
#ifdef NATIVE_CYCLE_PROFILING
        profile_frame_postprocess(&app_settings);
#endif

#ifdef PROFILING_CODEC_SAMPLES
      timer0=_timer0_read()-timer0;
      double number =ceil((double)16000/ read1);
      if (print_header_encoder)
      {
          printf("HEADER ENCODER: block_size=%d \n",read1);
          print_header_encoder=0;
      }
      //double number=10;
      //printf("timer %d\t mips %.2f \n",(timer0-encoder->private_->time_read),(((double)(timer0-encoder->private_->time_read)*number)/(1000000)));
      printf("timer_E %d\t E_mips %.2f \t",(timer0),(((double)(timer0)*number)/(1000000)));
#endif      
      /* Modify read1 to the expected number of encoded bitstream samples */
      read1 /= 2;

      /* Test for error */
      if (smpno!=read1)
	HARAKIRI("Error encoding!\n",10);
    }

    if (decode)
    {
#ifdef PROFILING_CODEC_SAMPLES
        static uint32_t print_header_decoder=1;
        uint32_t timer0;
        //decoder->private_->time_read=0;
        timer0=_timer0_read();
#endif

#ifdef NATIVE_CYCLE_PROFILING
        profile_frame_preprocess(&app_settings);
#endif
      /* Decode */
      smpno = g722_decode(cod_buf, outcode, mode, (short)read1, &decoder);

 
      /* Modify read1 to the expected no.of decoded reconstructed samples */
      read1 *= 2;

#ifdef NATIVE_CYCLE_PROFILING
     
          unsigned bytes_ps = 2;
          unsigned sample_rate = 16000;
          unsigned channels=1;
          unsigned blocksize=read1;
          unsigned size = blocksize*(bytes_ps)*channels;

          if(sample_rate != app_settings.stream_config.sample_rate || bytes_ps != app_settings.stream_config.sample_size)
          { 
              app_settings.stream_config.stream_name =  FileIn;
              app_settings.stream_config.stream_type = 0;
              app_settings.stream_config.component_class = ARC_API_CLASS_DECODER;
              app_settings.stream_config.sample_rate = sample_rate;
              app_settings.stream_config.sample_size = bytes_ps;
              app_settings.stream_config.bit_rate = 0;
              app_settings.stream_config.num_ch = channels;
              app_settings.stream_config.codec_instance_size = sizeof(g722_state )*2;
          }

          app_settings.rt_stats.microseconds_per_frame = (unsigned)1000000*blocksize/sample_rate;
          app_settings.rt_stats.used_output_buffer = size;
      
      profile_frame_postprocess(&app_settings);
#endif

#ifdef PROFILING_CODEC_SAMPLES
      timer0=_timer0_read()-timer0;
      double number =ceil((double)16000/ read1);
      if (print_header_decoder)
      {
          printf("\nHEADER DECODER: block_size=%d mode=%d \n",read1, mode);
          print_header_decoder=0;
      }
      //double number=10;
      //printf("timer %d\t mips %.2f \n",(timer0-encoder->private_->time_read),(((double)(timer0-encoder->private_->time_read)*number)/(1000000)));
      printf("timer_D %d\t D_mips %.2f \t",(timer0),(((double)(timer0)*number)/(1000000)));
#endif
      /* Test for error */
      if (smpno!=read1)
	HARAKIRI("Error decoding!\n",10);
    }

#ifdef PROFILING_CODEC_SAMPLES
    printf("\n");
#endif
    /* Update sample counter */
    iter += smpno;

    /* Save bitstream or decoded samples */
    if (fwrite(out_buf, sizeof (Word16), read1, out) != (size_t)read1)
      KILL(FileOut,-4);
  }

  /* Free memory */
#ifndef STATIC_ALLOCATION
  if (encode && decode)
  {
    free(cod_buf);
    free(out_buf);
    free(inp_buf);
  }
  else if (encode)
  {
    /* Free only input and bitstream buffers */
    free(cod_buf);
    free(inp_buf);
  }
  else
  {
    /* Free only bitstream and output buffers */
    free(out_buf);
    free(cod_buf);
  }
#endif

  /* Close input and output files */
  fclose(out);
  fclose(inp);

#ifdef NATIVE_CYCLE_PROFILING
  profile_postprocess(&app_settings);
#endif

  /* Exit with success for non-vms systems */
#ifndef VMS
  return (0);
#endif
}
