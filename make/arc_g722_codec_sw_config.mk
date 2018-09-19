#####################################################################
#####################################################################
##
##    Software configuration
##    Synopsys.
##    http://www.synopsys.com/
##
##    Copyright (c) Synopsys, 2013
##    All rights reserved
##
#####################################################################
#####################################################################

# possible components:
# G722_CODEC   - build G722 demo aplication , decoder test aplication and  encoder test aplication
# G722_ENCODER - build only G722 encoder test aplication
# G722_DECODER - build only G722 decoder test aplication
COMPONENT?=G722_CODEC
NATIVE_STACK_PROFILING ?= on
NATIVE_CYCLE_PROFILING ?= on
NATIVE_BUSBANDWIDTH_PROFILING ?= off
#
# profile local mode  enable/disable
#
ifeq ($(NATIVE_CYCLE_PROFILING),on)
override CFLAGS   += -DNATIVE_CYCLE_PROFILING
override APP_CFLAGS   += -DNATIVE_CYCLE_PROFILING
endif

ifeq ($(NATIVE_STACK_PROFILING),on)
override CFLAGS   += -DNATIVE_STACK_PROFILING
override APP_CFLAGS   += -DNATIVE_STACK_PROFILING
endif
PROFILE_LOCAL_MODE?=off

override CFLAGS += -Hon=Long_enums -Hsdata0 -Hlto


override CPPFLAGS +=
override ASMFLAGS += -Hasopt=-c -Hasmcpp -D$(COMPONENT)
override LDFLAGS  += 