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

#
# profile local mode  enable/disable
#
PROFILE=on
PROFILE_LOCAL_MODE?=off

override CFLAGS += -Hon=Long_enums -Hsdata0 -Hlto


override CPPFLAGS +=
override ASMFLAGS += -Hasopt=-c -Hasmcpp -D$(COMPONENT)
override LDFLAGS  += 