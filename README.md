# G722
This is the G.722 optimized for ARC EM cores with DSP functionality (ARC EMxD cores: https://www.synopsys.com/designware-ip/processor-solutions/arc-processors/arc-em-family.html). 
The original G722 code was taken from G.191 (https://www.itu.int/rec/T-REC-G.191) because https://www.itu.int/rec/T-REC-G.722 assumes wide-band codecs (aka G.722.2). The patch was applied over vanilla G.722 to make it ARC EM friendly. Please note, modification are touching (and removing) some files in folders: basop, eid, g722 and utl.

# Prerequisites
1. Installed MetaWare Development Toolkit M-2017.03 or newer 
2. Valid license for MetaWare Development compiler and debugger 

# Build
These steps allow you to build and run test application for ARC EM 11D (https://www.synopsys.com/dw/ipdir.php?ds=arc-em9d-em11d):
1. Open command prompt or terminal
2. Go to folder 'make' folder 
3. Type `gmake clean all` to rebuild libraries and application. You may see some warning messages during build, please ignore them. In result you will have g722demo.elf executable in your folder 
4. Type `gmake run` to start G.722 application over ARC nSIM simulator in command line. In result, encoder will produce file 'output.bit' and decoder will produce file 'output.pcm'

# Customizing for your hardware platform 
If you would build and run codec for your specific platform you need to modify 'hw_config.mk' in make folder. At least you need to specify path to your TCF file. After that you need to rebuild codec completely.
