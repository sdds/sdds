SDDS_TOPDIR := ../..


ifeq ($(BUILD),sensor_board)
  SDDS_OBJDIR := objs-128rfa1
  TARGET := avr-atmega128rfa1
else
  SDDS_OBJDIR := objs-1284p
  TARGET := avr-raven
endif

SDDS_PLATFORM := contiki
SDDS_ARCH := atmega

APPLICATION_NAME := mywrite

DATASTRUCTURES_FILE := datastructures

# Object files of the generateted dds data types
#DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/power_cur-ds.o
DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/DoorSensor-ds.o
DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/LightSensor-ds.o
DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/MovementSensor-ds.o
DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/OnOffSwitch-ds.o
DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/TemperatureAndHumiditySensor-ds.o
DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/TemperatureSensor-ds.o

# object files depending on driver for sensors
#  DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-adc.o
DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-AMN31112.o
DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-TSL2561.o
DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-twi.o
DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-LED.o


# object files of the generates implementation code file of sdds
IMPL_DEPEND_OBJS = $(SDDS_OBJDIR)/atmega_sdds_impl.o

# file for the preprocessor constants of sdds
SDDS_CONSTANTS_FILE := ./atmega_constants.h


ALL_OBJS += $(DRIVER_DEPEND_OBJS)
ALL_OBJS += $(IMPL_DEPEND_OBJS)
ALL_OBJS += $(SDDS_OBJDIR)/$(APPLICATION_NAME).o
ALL_OBJS += $(DATA_DEPEND_OBJS)

ifneq ($(SDDS_CONTIKI_PORT),)
  CFLAGS += -DSDDS_CONTIKIPORT=$(SDDS_CONTIKI_PORT)
endif
ifneq ($(SDDS_CONTIKI_LISTEN_ADDRESS),)
  CFLAGS += -DSDDS_CONTIKI_LISTEN_ADDRESS=\"$(SDDS_CONTIKI_LISTEN_ADDRESS)\"
endif
ifneq ($(SDDS_CONTIKI_SEND_ADDRESS),)
  CFLAGS += -DSDDS_CONTIKI_SEND_ADDRESS=\"$(SDDS_CONTIKI_SEND_ADDRESS)\"
endif
ifneq ($(USE_DERFMEGA128),)
 CFLAGS += -DUSE_DERFMEGA128
endif




include $(SDDS_TOPDIR)/sdds.mk


include $(CONTIKI)/Makefile.include



DATA_DEPEND_SRCS += $(patsubst $(SDDS_OBJDIR)/%.o,%.c,$(DATA_DEPEND_OBJS))
DATA_DEPEND_SRCS += $(patsubst $(SDDS_OBJDIR)/%.o,%.h,$(DATA_DEPEND_OBJS))
CLEAN += $(DATA_DEPEND_SRCS)

IMPL_DEPEND_SRCS += $(patsubst $(SDDS_OBJDIR)/%.o,%.c,$(IMPL_DEPEND_OBJS))
IMPL_DEPEND_SRCS += $(patsubst $(SDDS_OBJDIR)/%.o,%.h,$(IMPL_DEPEND_OBJS))
CLEAN += $(IMPL_DEPEND_SRCS)

CLEAN += $(ALL_OBJS)
CLEAN += $(patsubst %.o,%.d,$(ALL_OBJS))
CLEAN += $(SDDS_CONSTANTS_FILE)

%-ds.c %-ds.h: $(DATASTRUCTURES_FILE)
	$(shell python $(SDDS_TOPDIR)/generate_ds.py $<)

%_sdds_impl.c %_sdds_impl.h: %-dds-roles $(DATASTRUCTURES_FILE) $(DATA_DEPEND_SRCS)
	$(shell python $(SDDS_TOPDIR)/generate_sdds.py $(<:-dds-roles=))

all:

CFLAGS += -I.


$(SDDS_OBJDIR)/%.o: %.c
	$(COMPILE.c) -MMD $(OUTPUT_OPTION) $<

$(APPLICATION_NAME).c: $(APPLICATION_NAME).c $(IMPL_DEPEND_SRCS) $(DATA_DEPEND_SRCS)
	$(CC) $(CFLAGS) -MM -MF$(SDDS_OBJDIR)/$(APPLICATION_NAME).d -MT$@ $^


$(APPLICATION_NAME).elf: $(APPLICATION_NAME).co $(SDDS_OBJS) $(IMPL_DEPEND_OBJS) $(DATA_DEPEND_OBJS) $(DRIVER_DEPEND_OBJS) contiki-$(TARGET).a
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^ $(LIBC)

%.hex: %.elf
	$(OBJCOPY) $^ -j .text -j .data -O ihex $@

%.ihex: %.elf
	$(OBJCOPY) $^ -O ihex $@

flash:
	avrdude  -pm128rfa1 -cdragon_jtag -Pusb -U flash:w:$(APPLICATION_NAME).hex
#	avarice -g -e -p -f $(APPLICATION_NAME).hex $(FLASH_ARGS)

debug:
	avr-gdb $(TARGET)-write.elf

CLEAN += $(APPLICATION_NAME).elf $(APPLICATION_NAME).hex $(APPLICATION_NAME).ihex $(APPLICATION_NAME).out
CLEAN += symbols.c symbols.h
CLEAN += $(APPLICATION_NAME).d


-include $(patsubst %.o,%.d,$(ALL_OBJS))
