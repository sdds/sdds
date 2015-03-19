SDDS_TOPDIR := ../..

APPLICATIONS_DIR := ../WieDAS-Apps/
#APPS=hexabus-webserver udp_handler mdns_responder button metering relay provisioning
APPS=button metering relay provisioning
UIP_CONF_IPV6=1

#ifeq ($(BUILD),sensor_board)
#  SDDS_OBJDIR := objs-128rfa1
#  TARGET := avr-atmega128rfa1
#else
#  SDDS_OBJDIR := objs-1284p
#  TARGET := avr-raven
#endif
SDDS_OBJDIR := objs-1284p
TARGET := Hexabus-Socket

SDDS_PLATFORM := contiki
SDDS_ARCH := atmega

APPLICATION_NAME := mywrite

DATASTRUCTURES_FILE := datastructures

CFLAGS += -I.
CFLAGS += -I $(APPLICATIONS_DIR)
#CFLAGS += -Werror
#CFLAGS += -Wno-error=unused-variable  



# Object files of the generateted dds data types
#DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/power_cur-ds.o
#DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/PowerMeter-ds.o
#DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/PowerMeterAverage-ds.o
#DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/OnOffOutput-ds.o
#DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/OnOffFunctionality-ds.o
#DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/SimpleLamp-ds.o
#DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/ToggleFunctionality-ds.o

# object files depending on driver for sensors
# DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-adc.o

include Dependency.mk

APP_DEPEND_OBJS += $(SDDS_OBJDIR)/Wiedas_WallnodeApp.o

# object files of the generates implementation code file of sdds
IMPL_DEPEND_OBJS = $(SDDS_OBJDIR)/atmega_sdds_impl.o

# file for the preprocessor constants of sdds
SDDS_CONSTANTS_FILE := ./atmega_constants.h


ALL_OBJS += $(DRIVER_DEPEND_OBJS)
ALL_OBJS += $(IMPL_DEPEND_OBJS)
ALL_OBJS += $(SDDS_OBJDIR)/$(APPLICATION_NAME).o
ALL_OBJS += $(DATA_DEPEND_OBJS)
ALL_OBJS += $(APP_DEPEND_OBJS)
ALL_OBJS += $(APPLICATION_DEPEND_OBJS)

ifneq ($(PLATFORM_CONTIKI_SDDS_PORT),)
  CFLAGS += -DSDDS_CONTIKIPORT=$(PLATFORM_CONTIKI_SDDS_PORT)
endif
ifneq ($(PLATFORM_CONTIKI_SDDS_LISTEN_ADDRESS),)
  CFLAGS += -DPLATFORM_CONTIKI_SDDS_LISTEN_ADDRESS=\"$(PLATFORM_CONTIKI_SDDS_LISTEN_ADDRESS)\"
endif
ifneq ($(PLATFORM_CONTIKI_SDDS_SEND_ADDRESSS),)
  CFLAGS += -DPLATFORM_CONTIKI_SDDS_SEND_ADDRESSS=\"$(PLATFORM_CONTIKI_SDDS_SEND_ADDRESSS)\"
endif
ifneq ($(CONFIG_USE_DERFMEGA128),)
 CFLAGS += -DCONFIG_USE_DERFMEGA128
endif




include $(SDDS_TOPDIR)/sdds.mk


include $(CONTIKI)/Makefile.include

APPLICATIONS_DEPEND_SRCS += $(patsubst $(SDDS_OBJDIR)/%.o,$(APPLICATIONS_DIR)/%.c,$(APPLICATION_DEPEND_OBJS))


DATA_DEPEND_SRCS += $(patsubst $(SDDS_OBJDIR)/%.o,%.c,$(DATA_DEPEND_OBJS))
DATA_DEPEND_SRCS += $(patsubst $(SDDS_OBJDIR)/%.o,%.h,$(DATA_DEPEND_OBJS))
CLEAN += $(DATA_DEPEND_SRCS)

IMPL_DEPEND_SRCS += $(patsubst $(SDDS_OBJDIR)/%.o,%.c,$(IMPL_DEPEND_OBJS))
IMPL_DEPEND_SRCS += $(patsubst $(SDDS_OBJDIR)/%.o,%.h,$(IMPL_DEPEND_OBJS))
CLEAN += $(IMPL_DEPEND_SRCS)

CLEAN += $(APPLICATION_DEPEND_OBJS)
CLEAN += $(APP_DEPEND_OBJS)

CLEAN += $(ALL_OBJS)
CLEAN += $(patsubst %.o,%.d,$(ALL_OBJS))
CLEAN += $(SDDS_CONSTANTS_FILE)

%-ds.c %-ds.h: $(DATASTRUCTURES_FILE)
	$(shell python $(SDDS_TOPDIR)/generate_ds.py $<)

%_sdds_impl.c %_sdds_impl.h: %-dds-roles $(DATASTRUCTURES_FILE) $(DATA_DEPEND_SRCS)
	$(shell python $(SDDS_TOPDIR)/generate_sdds.py $(<:-dds-roles=) $(DATASTRUCTURES_FILE))

all:


$(SDDS_OBJDIR)/%.o: $(APPLICATIONS_DIR)/%.c 
	$(COMPILE.c) -MMD $(OUTPUT_OPTION) $<

$(SDDS_OBJDIR)/%.o: %.c
	$(COMPILE.c) -MMD $(OUTPUT_OPTION) $<

$(APPLICATION_NAME).c: $(APPLICATION_NAME).c $(IMPL_DEPEND_SRCS) $(DATA_DEPEND_SRCS) $(APPLICATION_DEPEND_OBJS)
	$(CC) $(CFLAGS) -MM -MF$(SDDS_OBJDIR)/$(APPLICATION_NAME).d -MT$@ $^


$(APPLICATION_NAME).elf: $(APPLICATION_NAME).co $(SDDS_OBJS) $(IMPL_DEPEND_OBJS) $(APP_DEPEND_OBJS) $(APPLICATION_DEPEND_OBJS) $(DATA_DEPEND_OBJS) $(DRIVER_DEPEND_OBJS) contiki-$(TARGET).a
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^ $(LIBC)

%.hex: %.elf
	$(OBJCOPY) $^ -j .text -j .data -O ihex $@

%.ihex: %.elf
	$(OBJCOPY) $^ -O ihex $@

flash: $(APPLICATION_NAME).hex
	avrdude -pm1284p -cdragon_jtag -Pusb -U flash:w:$(APPLICATION_NAME).hex -F
#	avarice -g -e -p -f $(APPLICATION_NAME).hex $(FLASH_ARGS)

debug:
	avr-gdb $(TARGET)-write.elf

CLEAN += $(APPLICATION_NAME).elf $(APPLICATION_NAME).hex $(APPLICATION_NAME).ihex $(APPLICATION_NAME).out
CLEAN += symbols.c symbols.h
CLEAN += $(APPLICATION_NAME).d


-include $(patsubst %.o,%.d,$(ALL_OBJS))
# DO NOT DELETE
