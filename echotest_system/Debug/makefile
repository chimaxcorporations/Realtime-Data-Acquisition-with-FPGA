#
# This file was created by Xilinx Vitis tool.
#


#
# tools
#

RM = rm -f
RMDIR = rm -rf



#
# variables
#

SDCARD_GEN = sdcard_gen
GENERATE_BIF = ::scw::generate_bif

XPFM_PATH = C:/Users/c.eguzo/Desktop/C-EGUZO-PHD-FZ/GENERAL/Udacity_project/ADC_Project/deletefolder/design_1_wrapper/export/design_1_wrapper/design_1_wrapper.xpfm
SYS_CONFIG = design_1_wrapper
DOMAINS = domain_ps7_cortexa9_0
BIF_PATH = C:/Users/c.eguzo/Desktop/C-EGUZO-PHD-FZ/GENERAL/Udacity_project/ADC_Project/deletefolder/echotest_system/Debug/system.bif

BITSTREAM_PATH = C:/Users/c.eguzo/Desktop/C-EGUZO-PHD-FZ/GENERAL/Udacity_project/ADC_Project/deletefolder/echotest/_ide/bitstream/design_1_wrapper.bit
SD_FILES = --elf C:/Users/c.eguzo/Desktop/C-EGUZO-PHD-FZ/GENERAL/Udacity_project/ADC_Project/deletefolder/echotest/Debug/echotest.elf,ps7_cortexa9_0

#
# primary build targets
#

all: package

package:
	-@echo Generating bif file for the system project
	generate_system_bif.bat 62103 ${XPFM_PATH} ${DOMAINS} ${BIF_PATH}
	${SDCARD_GEN} --xpfm ${XPFM_PATH} --sys_config ${SYS_CONFIG} --bif ${BIF_PATH} --bitstream ${BITSTREAM_PATH} ${SD_FILES}

clean:
	$(RMDIR) ./_sds
	${RMDIR} package
	${RM} system.bif

.PHONY: all package clean


