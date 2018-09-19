#####################################################################
#####################################################################
##
## Make File for G722 Codec Applications
##    Synopsys.
##    http://www.synopsys.com/
##
##    Copyright (c) Synopsys, 2013
##    All rights reserved
##
#####################################################################
#####################################################################

#################################################
# Include external configuration files
CONFIG_DIR ?= .

include common.mk
#include rtc_config.mak
include $(CONFIG_DIR)$(PS)hw_config.mk
include $(CONFIG_DIR)$(PS)arc_g722_codec_sw_config.mk


override CFLAGS +=   -Hnocopyr -Hnocplus -Hon=Behaved -D$(COMPONENT) 
override CFLAGS += -Xdsp_ctrl=noguard,up,preshift -Hfxapi -Hitut
override CFLAGS += -DSTATIC_ALLOCATION -Hheap=4K -Hstack=16K -Hcl -Hcrt_argv
#override CFLAGS += -DAS210 -DARC_X_BASE_ADDR=0x00f80000 -DARC_Y_BASE_ADDR=0x00fa0000  -DWORKLOAD_MEASUREMENT

#
# profile local mode  enable/disable
#
ifeq ($(PROFILE_LOCAL_MODE),on)
override CFLAGS += -DPROFILING_CODEC_SAMPLES
endif

#################################################
# Default target global settings

# Library destination directory
# (resulting library file will be placed into 
#  $BINARY_DIR by default)
BINARY_DIR  ?= ..$(PS)lib

# Build files (temporaty and OBJ-files) directory
BUILD_DIR   ?= ..$(PS)obj

#################################################
# Targets list

.PHONY: all clean cleanall 

#################################################
# Encoder UTA parameters section
override APP_CFLAGS +=

APP_NAME = g722demo.elf
MINI_APP_NAME = g722_enc_mini.elf

ifneq (, $(filter -DG722_ENCODER%, $(CFLAGS)))
    APP_NAME =tstcg722.elf
    MINI_APP_NAME = g722_enc_mini.elf
endif

ifneq (, $(filter -DG722_DECODER%, $(CFLAGS)))
    APP_NAME =tstdg722.elf
    MINI_APP_NAME = g722_dec_mini.elf
endif



ifneq (, $(filter -DG722_CODEC%, $(CFLAGS)))
    CLEAN_BINS += g722demo.elf
    MAP_NAME   += g722demo.map
    CLEAN_BINS += tstcg722.elf
    MAP_NAME   += tstcg722.map
    CLEAN_BINS += tstdg722.elf
    MAP_NAME   += tstdg722.map
    CLEAN_BINS += g722_dec_mini.elf
    MAP_NAME   += g722_dec_mini.map
    CLEAN_BINS += g722_enc_mini.elf
    MAP_NAME   += g722_enc_mini.map
else
    CLEAN_BINS += $(APP_NAME)
    MAP_NAME = $(call get_file_name, $(APP_NAME)).map
    CLEAN_BINS += $(MINI_APP_NAME)
    MAP_NAME   += $(call get_file_name, $(MINI_APP_NAME)).map
endif

# Setting target
all: $(APP_NAME) 


ifneq (, $(filter -DG722_ENCODER%, $(CFLAGS)))
run: 
	mdb -run -tcf=$(TCF) g722demo.elf -enc inpsp.bin output.bit -raw_profiling_data enc.prof
endif

ifneq (, $(filter -DG722_DECODER%, $(CFLAGS)))
run: 
	mdb -run -tcf=$(TCF) g722demo.elf -dec output.bit output.pcm -raw_profiling_data dec.prof
endif    



APP_INCLUDE_DIRS_COMMON = \
    ..$(PS)g722 \
    ..$(PS)basop \
    ..$(PS)utl \
    ..$(PS)eid \
    ..$(PS)test$(PS)testlib$(PS)include

    
APP_INCLUDE_DIRS_ENCODER = 
    
APP_INCLUDE_DIRS_DECODER = 

APP_C_FILES_DIRS_COMMON = \
    ..$(PS)g722 \
    ..$(PS)utl \
    ..$(PS)test$(PS)testlib
	
APP_C_FILES_DIRS_ENCODER = \
    
    
APP_C_FILES_DIRS_DECODER = \
    

APP_C_FILES_COMMON =

APP_C_FILES_ENCODER = tstcg722.c

MINI_APP_C_FILES_ENCODER = 

APP_C_FILES_DECODER = tstdg722.c

MINI_APP_C_FILES_DECODER = 

APP_C_FILES_DEC_ENC = 
    
APP_C_FILES_CODEC = g722demo.c \
                    arc_profile.c \
                    helper_lib.c
#CDK_workload.c genericStds.c

APP_CPP_FILES_DIRS   = 
APP_CPP_FILES        = 

APP_INCLUDE_DIRS = $(APP_INCLUDE_DIRS_COMMON)
APP_C_FILES_DIRS = $(APP_C_FILES_DIRS_COMMON)
APP_C_FILES = $(APP_C_FILES_COMMON)
MINI_APP_C_FILES         = 

ifneq (, $(filter -DG722_ENCODER%, $(CFLAGS)))
    APP_CFLAGS += -DUSE_CUSTOM_CMDLINE -DINPUT_CODE_CUSTOM -DOUTPUT_CODE_CUSTOM
    #-DCODEC_SPECIFIC_BUFFER_CONFIG

    APP_INCLUDE_DIRS += $(APP_INCLUDE_DIRS_ENCODER)
    APP_C_FILES_DIRS += $(APP_C_FILES_DIRS_ENCODER)
    APP_C_FILES += $(APP_C_FILES_ENCODER)
    APP_C_FILES += $(APP_C_FILES_DEC_ENC)
    MINI_APP_C_FILES += $(MINI_APP_C_FILES_ENCODER)
    CODEC_INTERFACE = -DGET_CODEC_INTERFACE=ARC_G722_ENC_get_interface
endif

ifneq (, $(filter -DG722_DECODER%, $(CFLAGS)))
    APP_INCLUDE_DIRS += $(APP_INCLUDE_DIRS_DECODER)
    APP_C_FILES_DIRS += $(APP_C_FILES_DIRS_DECODER)
    APP_C_FILES += $(APP_C_FILES_DECODER)
    APP_C_FILES += $(APP_C_FILES_DEC_ENC)
    MINI_APP_C_FILES += $(MINI_APP_C_FILES_DECODER)
    CODEC_INTERFACE = -DGET_CODEC_INTERFACE=ARC_G722_DEC_get_interface
endif


# Internal definitions (do not modify)
APP_INCS     = $(foreach dir,$(APP_INCLUDE_DIRS), $(addprefix -I,$(dir)))
APP_OBJS_C   = $(addprefix $(BUILD_DIR)$(PS), $(APP_C_FILES:.c=.o))
APP_OBJS_CPP = $(addprefix $(BUILD_DIR)$(PS), $(APP_CPP_FILES:.c=.o))
MINI_APP_OBJS_C   = $(addprefix $(BUILD_DIR)$(PS), $(MINI_APP_C_FILES:.c=.o))


ifneq (, $(filter -DG722_CODEC%, $(CFLAGS)))
    APP_INCLUDE_DIRS += $(APP_INCLUDE_DIRS_ENCODER)
    APP_C_FILES_DIRS += $(APP_C_FILES_DIRS_ENCODER)
    APP_INCLUDE_DIRS += $(APP_INCLUDE_DIRS_DECODER)
    APP_C_FILES_DIRS += $(APP_C_FILES_DIRS_DECODER)
    APP_C_FILES += $(APP_C_FILES_CODEC) 
    APP_C_FILES_CLEAN += $(APP_C_FILES_CODEC) $(APP_C_FILES_DECODER) $(APP_C_FILES_ENCODER)
	CLEAN_OBJS += $(addprefix $(BUILD_DIR)$(PS), $(APP_C_FILES_CLEAN:.c=.o))
	CLEAN_ASMS =  $(addprefix $(BUILD_DIR)$(PS), $(APP_C_FILES_CLEAN:.c=.s))
else
	CLEAN_OBJS += $(APP_OBJS_C)
	CLEAN_ASMS =  $(APP_OBJS_C:.o=.s)
endif

# Application target
$(MINI_APP_NAME): $(MINI_APP_OBJS_C) $(BUILD_DIR)$(PS)wav_lib.o $(BUILD_DIR)$(PS)pv_lib.o $(BUILD_DIR)$(PS)api_lib.o $(BUILD_DIR)$(PS)arc_audio_api_helper_lib.o $(CODEC_LIB) arc600.lnk
	@echo [LD] [$(BUILD_TYPE_MSG) build] [Linking ELF output binary $@] 
	@$(LD) $(LDFLAGS) -Hldopt=-Csections,-Coutput=$(call get_file_name, $@).map -o $@ $^

$(MINI_APP_OBJS_C): $(BUILD_DIR)$(PS)%.o: %.c
	@echo Compiling C [opt SIZE]: $(CC) $< ...
	@$(CC) $(APP_INCS) $(CFLAGS) $(APP_CFLAGS) $(FLAGS_OPT_SIZE) $(CODEC_INTERFACE) -c $< -o $@
	
$(APP_NAME): $(APP_OBJS_C) $(APP_OBJS_CPP) $(CODEC_LIB)
	@echo [LD] [$(BUILD_TYPE_MSG) build] [Linking ELF output binary $@] $(APP_OBJS_C) $(APP_OBJS_CPP) $(CODEC_LIB)
	@$(LD) $(LDFLAGS) $(CFLAGS) -Hldopt=-Csections,-Coutput=$(call get_file_name, $@).map -o $@ $^

$(APP_OBJS_C): $(BUILD_DIR)$(PS)%.o: %.c
	@echo Compiling C [opt SIZE]: $(CC) $< ...
	@$(CC) $(APP_INCS) $(CFLAGS) $(APP_CFLAGS) $(FLAGS_OPT_SIZE) $(CODEC_INTERFACE) -c $< -o $@

$(APP_OBJS_CPP): $(BUILD_DIR)$(PS)%.o: %.cpp
	@echo Compiling C++ [opt SPEED]: $(CC) $< ...
	@$(CC) $(APP_INCS) $(CPPFLAGS) $(APP_CFLAGS) $(FLAGS_OPT_SIZE) $(CODEC_INTERFACE) -c $< -o $@

	
##################################################
# Global VPATH settings

vpath %.c   $(APP_C_FILES_DIRS)
vpath %.cpp $(APP_CPP_FILES_DIRS)
vpath %.h   $(APP_INCLUDE_DIRS)

clean:
	@echo [Deleting OBJ files] $(COMPONENT) $(CLEAN_OBJS) $(CLEAN_ASMS)
	@$(RM) $(call fix_platform_path, $(CLEAN_OBJS))
	@$(RM) $(call fix_platform_path, $(CLEAN_ASMS))

cleanall: clean
	@echo [Deleting binary files] $(COMPONENT) $(CLEAN_BINS)
	@$(RM) $(call fix_platform_path, $(CLEAN_BINS))
	@$(RM) $(call fix_platform_path, $(MAP_NAME))
