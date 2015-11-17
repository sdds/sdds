SDDS_TOPDIR := ../../
FREERTOS_TOPDIR := ../../../platform/esp-open-rtos/FreeRTOS/

SDDS_PLATFORM := freertos
SDDS_ARCH := esp
SDDS_OBJDIR := build/objs-esp

LOCAL_CONSTANTS := local_constants.h
SDDS_CONSTANTS_FILE := ./gen_constants.h

$(LOCAL_CONSTANTS):
	touch $(LOCAL_CONSTANTS)
	
$(SDDS_OBJDIR):
	mkdir -p $(SDDS_OBJDIR)

CLEAN := $(APPLICATION_NAME).hex
CLEAN += *-ds.c *-ds.h
CLEAN += *_sdds_impl.c *_sdds_impl.h
CLEAN += gen_constants.h

CFLAGS += -I $(FREERTOS_TOPDIR)/Source/include/
CFLAGS += -I $(FREERTOS_TOPDIR)/../lwip/lwip/src/include/
CFLAGS += -I $(FREERTOS_TOPDIR)/../lwip/include/
CFLAGS += -I $(FREERTOS_TOPDIR)/../include/
CFLAGS += -I $(FREERTOS_TOPDIR)/../lwip/lwip/src/include/$(PLATTFORM_RTOS_IP_PROTOCOL)/
CFLAGS += -Os

include $(SDDS_TOPDIR)/sdds.mk

PROGRAM=$(APPLICATION_NAME)
include ../../../platform/esp-open-rtos/common.mk

gen: $(LOCAL_CONSTANTS) $(SDDS_OBJDIR)
	$(shell generate.sh)
	
$(APPLICATION_NAME).hex: all
	$(OBJCOPY) build/$(APPLICATION_NAME).out -j .text -j .data -O ihex $@
