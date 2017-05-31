#####################################################################
#####################################################################
##
##    Common toolchain and OS-specific features configuration file
##    Synopsys.
##    http://www.synopsys.com/
##
##    Copyright (c) Synopsys, 2013
##    All rights reserved
##
#####################################################################
#####################################################################

#################################################
# Toolchain settings

# Tools
CC = ccac
LD = ccac
LR = ldac
AS = ccac
MD = ccac
SZ = sizeac
AR = arac
MKDIR = mkdir


DEBUG_MODE?=off

#################################################
# Platform settings

ifneq ($(ComSpec)$(COMSPEC),)
    O_SYS=Windows
    PS=/
    RM=del /Q
    fix_platform_path = $(subst /,\$(nullstring), $(1))
    MAKE = gmake
else
    O_SYS=Unix
    PS=/
    RM=rm -f
    fix_platform_path = $(1)
    MAKE = make
endif

# Optimization flags

ifeq ($(DEBUG_MODE),off)
    FLAGS_OPT_SPEED = -O3 -g -Hkeepasm -Hanno
    FLAGS_OPT_SIZE  = -Os -g  -Hkeepasm -Hanno 
else
    FLAGS_OPT_SPEED = -g -Hkeepasm -Hanno
    FLAGS_OPT_SIZE  = -g -Hkeepasm -Hanno
    override CFLAGS += -DARC_API_USE_ASSERT
endif

# Helper functions
get_file_name = $(basename $(notdir $(1)))
get_file_ext  = $(suffix $(notdir $(1)))
