#####################################################################
#####################################################################
##
##    Hardware platform configuration 
##    Synopsys.
##    http://www.synopsys.com/
##
##    Copyright (c) Synopsys, 2013
##    All rights reserved
##
#####################################################################
#####################################################################
# Hardware configuration refers to tcf file
# the following lines should be edited manually:
#  - provide path to the tcf file with the required hardware configuration
#    example: override CFLAGS += -tcf=".$(PS)arc.tcf"


#standart template
#TCF ?= em7d_voice_audio
#TCF ?= em9d_voice_audio
TCF ?= em11d_voice_audio


# For MetaWare 2015.09 or newer
#override CFLAGS += -tcf=$(TCF) -Xdsp_itu -tcf_core_config
override CFLAGS += -tcf=$(TCF)  -tcf_core_config

# STAR 9000768386 workaround (only for MetaWare older than 2015.09):
# If data cache exists:
#  - provide actual data cache line size (in bytes)
#    example: override CFLAGS += -tcf=$(TCF) -DCACHE_LINE_SIZE=32
# If no data cache:
#  - provide default (dummy) data cache line size (in bytes) equal to 4
#  - define macro DCACHE_DISABLED
#    example: override CFLAGS += -tcf=$(TCF) -DCACHE_LINE_SIZE=4 -DDCACHE_DISABLED

# Old style low level hardware configuration is possible but not recommended.
# If you are not using TCF files, you should explicitly add macros like -DCACHE_LINE_SIZE=<data_cache_line_size_in_bytes> (if data cache exists) or -DCACHE_LINE_SIZE=4 -DDCACHE_DISABLED (if no data cache)
# Example:
#override CFLAGS += -arcv2em -core1 -HL -Hpc_width=24 -Xcode_density -Xswap -Xnorm -Xmpy_option=mpyd -Xshift_assist -Xbarrel_shifter -Xdsp1 -Xdsp_complex -Xtimer0 -Hccm -DCACHE_LINE_SIZE=4 -DDCACHE_DISABLED
 
override CFLAGS += -DCACHE_LINE_SIZE=32 


