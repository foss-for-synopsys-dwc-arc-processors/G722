#####################################################################
#####################################################################
##
## Make File for G722 codec Library
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
include $(CONFIG_DIR)$(PS)hw_config.mk
include $(CONFIG_DIR)$(PS)arc_g722_codec_sw_config.mk

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

.PHONY: all lib clean cleanall app
all: lib app

#################################################
# Library parameters section

override CFLAGS +=  -Hnocopyr -Hnocplus -Hon=Behaved -D$(COMPONENT) 
override CFLAGS += -Hon=Each_function_in_own_section  -Hon=Each_var_in_own_section
override CFLAGS += -Xdsp_ctrl=noguard,up,preshift -Hfxapi -Hitut
#workaround for new functions L_mult0 and L_mac0 until updated basop32.h 
#override CFLAGS += -DWORKAROUND_BASOP

ifeq ($(METAWARE_ROOT),)
$(error METAWARE_ROOT environment variable should be defined as path to Metaware Tools root directory)
endif
#################################################
# Library parameters section


LIB_NAME = arc_g722_codec


#
#if we set optimization to CFLAGS with key -O unset FLAG_OPT_xxxxx from common.mk
#
ifneq (, $(filter -O%, $(CFLAGS)))
FLAGS_OPT_SIZE=
FLAGS_OPT_SPEED=
endif

ifneq (, $(filter -DG722_CODEC%, $(CFLAGS)))
	MAKE_APP = $(MAKE) -f Makefile_app.mk CODEC_LIB=$(BINARY_DIR)$(PS)$(LIB_NAME).a COMPONENT=G722_CODEC
	MAKE_CL  = $(MAKE) -f Makefile_app.mk clean
	MAKE_APP2 = $(MAKE) -f Makefile_app.mk CODEC_LIB=$(BINARY_DIR)$(PS)$(LIB_NAME).a COMPONENT=G722_ENCODER
	MAKE_CL  = $(MAKE) -f Makefile_app.mk clean
	MAKE_APP3 = $(MAKE) -f Makefile_app.mk CODEC_LIB=$(BINARY_DIR)$(PS)$(LIB_NAME).a COMPONENT=G722_DECODER
else
	MAKE_APP = $(MAKE) -f Makefile_app.mk CODEC_LIB=$(BINARY_DIR)$(PS)$(LIB_NAME).a
	MAKE_CL  = 
	MAKE_APP2=
endif


# Setting target
lib: $(BUILD_DIR) $(BINARY_DIR) $(BINARY_DIR)$(PS)$(LIB_NAME).lib $(BINARY_DIR)$(PS)$(LIB_NAME).a
CLEAN_BINS += $(BINARY_DIR)$(PS)$(LIB_NAME).lib $(BINARY_DIR)$(PS)$(LIB_NAME).a


LIB_ASMFLAGS = \
-Hasopt=-on=assume_short


LIB_INCLUDE_DIRS_API = \
    
	
LIB_INCLUDE_DIRS_COMMON = \
    ..$(PS)g722 \
    ..$(PS)basop \
    ..$(PS)utl \
    ..$(PS)eid \

    
    
LIB_C_FILES_DIRS_COMMON = \
    ..$(PS)g722 \
    ..$(PS)basop \
    ..$(PS)utl \
    ..$(PS)eid
    
    

LIB_C_FILES_OPT_SIZE_COMMON = \
    
   
LIB_C_FILES_OPT_SIZE_ENCODER = \
    


LIB_C_FILES_OPT_SPEED_COMMON = \
    count.c \
    enh1632.c \
    funcg722.c \
    g722.c \
    basop32.c 
   
    
 
LIB_C_FILES_OPT_SPEED_ENCODER = \
    

 
LIB_C_FILES_OPT_SPEED_DECODER = \
    


LIB_CPP_FILES_DIRS = 
LIB_CPP_FILES = 

LIB_ASM_FILES_DIRS_COMMON = \
    ..$(PS)common \
    ..$(PS)common$(PS)arc

LIB_ASM_FILES_DIRS_ENCODER = ..$(PS)enc
LIB_ASM_FILES_DIRS_DECODER = ..$(PS)dec 


LIB_ASM_FILES_COMMON = \
    
    
LIB_ASM_FILES_ENCODER = \
    

LIB_ASM_FILES_DECODER = \
    


LIB_INCLUDE_DIRS = $(LIB_INCLUDE_DIRS_API)

ifneq (, $(filter -DG722_ENCODER%, $(CFLAGS)))
#common
    LIB_INCLUDE_DIRS += $(LIB_INCLUDE_DIRS_COMMON)
    LIB_C_FILES_DIRS = $(LIB_C_FILES_DIRS_COMMON)
    LIB_ASM_FILES_DIRS = $(LIB_ASM_FILES_DIRS_COMMON)
    LIB_C_FILES_OPT_SIZE = $(LIB_C_FILES_OPT_SIZE_COMMON)
    LIB_C_FILES_OPT_SPEED = $(LIB_C_FILES_OPT_SPEED_COMMON)
    LIB_ASM_FILES = $(LIB_ASM_FILES_COMMON)
#encoder
    LIB_INCLUDE_DIRS += $(LIB_INCLUDE_DIRS_ENCODER)
    LIB_C_FILES_DIRS += $(LIB_C_FILES_DIRS_ENCODER)
    LIB_ASM_FILES_DIRS += $(LIB_ASM_FILES_DIRS_ENCODER)
    LIB_C_FILES_OPT_SIZE += $(LIB_C_FILES_OPT_SIZE_ENCODER)
    LIB_C_FILES_OPT_SPEED += $(LIB_C_FILES_OPT_SPEED_ENCODER)
    LIB_ASM_FILES += $(LIB_ASM_FILES_ENCODER)
endif

ifneq (, $(filter -DG722_DECODER%, $(CFLAGS)))
#common	
    LIB_INCLUDE_DIRS += $(LIB_INCLUDE_DIRS_COMMON)
    LIB_C_FILES_DIRS = $(LIB_C_FILES_DIRS_COMMON)
    LIB_ASM_FILES_DIRS = $(LIB_ASM_FILES_DIRS_COMMON)
    LIB_C_FILES_OPT_SIZE = $(LIB_C_FILES_OPT_SIZE_COMMON)    
    LIB_C_FILES_OPT_SPEED = $(LIB_C_FILES_OPT_SPEED_COMMON)
    LIB_ASM_FILES = $(LIB_ASM_FILES_COMMON)
#decoder
    LIB_INCLUDE_DIRS += $(LIB_INCLUDE_DIRS_DECODER)
    LIB_C_FILES_DIRS += $(LIB_C_FILES_DIRS_DECODER)
    LIB_ASM_FILES_DIRS += $(LIB_ASM_FILES_DIRS_DECODER)
    LIB_C_FILES_OPT_SIZE += $(LIB_C_FILES_OPT_SIZE_DECODER)
    LIB_C_FILES_OPT_SPEED += $(LIB_C_FILES_OPT_SPEED_DECODER)
    LIB_ASM_FILES += $(LIB_ASM_FILES_DECODER)
endif

ifneq (, $(filter -DG722_CODEC%, $(CFLAGS)))
#common
    LIB_INCLUDE_DIRS += $(LIB_INCLUDE_DIRS_COMMON)
    LIB_C_FILES_DIRS = $(LIB_C_FILES_DIRS_COMMON)
    LIB_ASM_FILES_DIRS = $(LIB_ASM_FILES_DIRS_COMMON)
    LIB_C_FILES_OPT_SIZE = $(LIB_C_FILES_OPT_SIZE_COMMON)    
    LIB_C_FILES_OPT_SPEED = $(LIB_C_FILES_OPT_SPEED_COMMON)
    LIB_ASM_FILES = $(LIB_ASM_FILES_COMMON)
#encoder
    LIB_INCLUDE_DIRS += $(LIB_INCLUDE_DIRS_ENCODER)
    LIB_C_FILES_DIRS += $(LIB_C_FILES_DIRS_ENCODER)
    LIB_ASM_FILES_DIRS += $(LIB_ASM_FILES_DIRS_ENCODER)
    LIB_C_FILES_OPT_SIZE += $(LIB_C_FILES_OPT_SIZE_ENCODER)
    LIB_C_FILES_OPT_SPEED += $(LIB_C_FILES_OPT_SPEED_ENCODER)
    LIB_ASM_FILES += $(LIB_ASM_FILES_ENCODER)
#decoder
    LIB_INCLUDE_DIRS += $(LIB_INCLUDE_DIRS_DECODER)
    LIB_C_FILES_DIRS += $(LIB_C_FILES_DIRS_DECODER)
    LIB_ASM_FILES_DIRS += $(LIB_ASM_FILES_DIRS_DECODER)
    LIB_C_FILES_OPT_SIZE += $(LIB_C_FILES_OPT_SIZE_DECODER)
    LIB_C_FILES_OPT_SPEED += $(LIB_C_FILES_OPT_SPEED_DECODER)
    LIB_ASM_FILES += $(LIB_ASM_FILES_DECODER)
endif


    SYMBOLS_FILE=g722_ext_symbols.txt


# Internal definitions (do not modify)
LIB_OBJS_C_OPT_SIZE   = $(addprefix $(BUILD_DIR)$(PS), $(LIB_C_FILES_OPT_SIZE:.c=.o))
LIB_OBJS_C_OPT_SPEED  = $(addprefix $(BUILD_DIR)$(PS), $(LIB_C_FILES_OPT_SPEED:.c=.o))
LIB_OBJS_CPP          = $(addprefix $(BUILD_DIR)$(PS), $(LIB_CPP_FILES:.cpp=.o))
LIB_OBJS_ASM          = $(addprefix $(BUILD_DIR)$(PS), $(LIB_ASM_FILES:.s=.o))

LIB_INCS              = $(foreach dir,$(LIB_INCLUDE_DIRS), $(addprefix -I,$(dir)))

LIB_OBJS_OPT_SPEED = $(LIB_OBJS_CPP_OPT_SPEED) $(LIB_OBJS_C_OPT_SPEED)
LIB_OBJS_OPT_SIZE =  $(LIB_OBJS_CPP_OPT_SIZE) $(LIB_OBJS_C_OPT_SIZE)
CLEAN_OBJS           += $(LIB_OBJS_C_OPT_SIZE) $(LIB_OBJS_C_OPT_SPEED) $(LIB_OBJS_CPP) $(LIB_OBJS_ASM)
CLEAN_ASMS            = $(LIB_OBJS_C_OPT_SIZE:.o=.s) $(LIB_OBJS_C_OPT_SPEED:.o=.s) $(LIB_OBJS_CPP:.o=.s)

# Directories creation
$(BUILD_DIR):
	@echo [MKDIR] [Creating directory $@]      
	@$(MKDIR) $(call fix_platform_path, $(BUILD_DIR))

$(BINARY_DIR):
	@echo [MKDIR] [Creating directory $@]      
	@$(MKDIR) $(call fix_platform_path, $(BINARY_DIR))

# Library linkage rule
$(BINARY_DIR)$(PS)$(LIB_NAME).lib: $(LIB_OBJS_C_OPT_SIZE) $(LIB_OBJS_OPT_SPEED) $(LIB_OBJS_CPP) $(LIB_OBJS_ASM)
	@echo [LR] [Linking dynamic output library $@]
	$(LR)  -zpurge -r -s -J $(SYMBOLS_FILE) -o $@ $^


$(BINARY_DIR)$(PS)$(LIB_NAME).a: $(LIB_OBJS_C_OPT_SIZE) $(LIB_OBJS_OPT_SPEED) $(LIB_OBJS_CPP) $(LIB_OBJS_ASM)
	@echo [LD] [Archiving static output library $@] 
	$(AR) -cq $@ $^

# Library sources compilation rules
ifneq (, $(filter -Hmerge%, $(CFLAGS)))
$(LIB_OBJS_OPT_SPEED): $(LIB_C_FILES_OPT_SPEED) $(LIB_CPP_FILES_OPT_SPEED)
	@echo Compiling C [Opt. SPEED with -Hmerge]: $(CC)  $< ...
	$(CC) $(LIB_INCS) $(LIB_CFLAGS) $(CFLAGS) $(FLAGS_OPT_SPEED) -Hmerge_exports_file=$(SYMBOLS_FILE) -Houtdir=$(BUILD_DIR) -c $^ -o $@
else
$(LIB_OBJS_C_OPT_SPEED): $(BUILD_DIR)$(PS)%.o: %.c
	@echo Compiling C [Opt. SPEED]: $(CC)  $< ...
	$(CC) $(LIB_INCS) $(LIB_CFLAGS) $(CFLAGS) $(FLAGS_OPT_SPEED) -c $< -o $@
endif
$(LIB_OBJS_C_OPT_SIZE): $(BUILD_DIR)$(PS)%.o: %.c
	@echo Compiling C [Opt. SIZE]: $(CC) $(CFLAGS) $< ...
	$(CC) $(LIB_INCS) $(LIB_CFLAGS) $(CFLAGS) $(FLAGS_OPT_SIZE) -c $< -o $@


$(LIB_OBJS_CPP): $(BUILD_DIR)$(PS)%.o: %.cpp
	@echo Compiling C++: $(CC) $< ...
	$(CC) $(LIB_INCS) $(LIB_CFLAGS) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(LIB_OBJS_ASM): $(BUILD_DIR)$(PS)%.o: %.s
	@echo Compiling ASM: $(AS) $< ...
	$(AS) $(LIB_INCS) $(LIB_ASMFLAGS) $(CFLAGS) $(ASMFLAGS) -c $< -o $@


#################################################
# Application section
app: lib
	$(MAKE_APP)
	$(MAKE_CL) 
	$(MAKE_APP2)
	$(MAKE_CL) 
	$(MAKE_APP3)

#################################################
# Size evaluation

run: app
	$(MAKE_APP2) run
	$(MAKE_APP3) run
    
size: $(BINARY_DIR)$(PS)$(LIB_NAME).lib
	@$(SZ) -f $<
	@echo This is all statically allocated memory, except XY, or memory to replace XY.
	@echo Dynamic data requirements are not included.

##################################################
# Global VPATH settings

vpath %.c   $(LIB_C_FILES_DIRS)
vpath %.cpp $(LIB_CPP_FILES_DIRS)
vpath %.h   $(LIB_INCLUDE_DIRS)
vpath %.s   $(LIB_ASM_FILES_DIRS)

clean: $(BUILD_DIR) $(BINARY_DIR)
	@echo [Deleting OBJ files]
	@$(RM) $(call fix_platform_path, $(CLEAN_OBJS))
	@$(RM) $(call fix_platform_path, $(CLEAN_ASMS))
	@$(MAKE) -f Makefile_app.mk clean

cleanall: clean $(BINARY_DIR)
	@echo [Deleting binary files]
	@$(RM) $(call fix_platform_path, $(CLEAN_BINS))
	@$(MAKE) -f Makefile_app.mk cleanall
