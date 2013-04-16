SDDS_TOPDIR := ../..

APPLICATIONS_DIR := ../WieDAS-Apps/


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

CFLAGS += -I.
CFLAGS += -I $(APPLICATIONS_DIR)
#CFLAGS += -Werror
#CFLAGS += -Wno-error=unused-variable  



# Object files of the generateted dds data types
#DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/power_cur-ds.o
# ausgelargert in Dependency.mk
#DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/DoorSensor-ds.o
#DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/LightSensor-ds.o
#DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/MovementSensor-ds.o
#DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/OnOffSwitch-ds.o
#DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/HumiditySensor-ds.o
#DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/TemperatureSensor-ds.o
#DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/DimmableLight-ds.o
#DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/LightRegulationFunctionality-ds.o
#DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/OnOffFunctionality-ds.o
#DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/SimpleLamp-ds.o
#DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/ToggleFunctionality-ds.o
#DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/WindowSensor-ds.o

# object files depending on driver for sensors
#  DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-adc.o
# ausgelargert in Dependency.mk


#DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-AMN31112.o
#DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-TSL2561.o
#DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-twi.o
#DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-LED.o
#DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-ds18x20.o
#DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-crc8.o
#DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-ow.o
#DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-AMN31112.o
#DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-GammaCorrection.o
#DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-GPIO_Input.o



APP_DEPEND_OBJS += $(SDDS_OBJDIR)/Wiedas_WallnodeApp.o


#APPLICATION_DEPEND_OBJS += $(SDDS_OBJDIR)/Wiedas_SensorApp_Light.o
#APPLICATION_DEPEND_OBJS += $(SDDS_OBJDIR)/Wiedas_SensorApp_Temperature.o
#APPLICATION_DEPEND_OBJS += $(SDDS_OBJDIR)/Wiedas_SensorApp_Movement.o
#APPLICATION_DEPEND_OBJS += $(SDDS_OBJDIR)/Wiedas_SensorApp_Humidity.o
#APPLICATION_DEPEND_OBJS += $(SDDS_OBJDIR)/Wiedas_SensorApp_DimmerLamp.o
#APPLICATION_DEPEND_OBJS += $(SDDS_OBJDIR)/Wiedas_SensorApp_Door.o
#APPLICATION_DEPEND_OBJS += $(SDDS_OBJDIR)/Wiedas_SensorApp_Switch.o
#APPLICATION_DEPEND_OBJS += $(SDDS_OBJDIR)/Wiedas_SensorApp_Window.o

include Dependency.mk


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


$(APPLICATION_NAME).elf: $(APPLICATION_NAME).co $(SDDS_OBJS) $(IMPL_DEPEND_OBJS) $(APP_DEPEND_OBJS) $(APPLICATION_DEPEND_OBJS) $(DATA_DEPEND_OBJS) $(DRIVER_DEPEND_OBJS)  contiki-$(TARGET).a
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^ $(LIBC)

%.hex: %.elf
	$(OBJCOPY) $^ -j .text -j .data -O ihex $@

%.ihex: %.elf
	$(OBJCOPY) $^ -O ihex $@

flash:
	avrdude  -pm128rfa1 -cdragon_jtag -Pusb -U flash:w:$(APPLICATION_NAME).hex
#	avarice -g -e -p -f $(APPLICATION_NAME).hex $(FLASH_ARGS)

fuse:
	 avrdude -pm128rfa1 -cdragon_jtag -Pusb -Ulfuse:w:0xe2:m -Uhfuse:w:0x91:m -Uefuse:w:0xfe:m
	 
debug:
	avr-gdb $(TARGET)-write.elf

test:
	echo $(WIEDAS_DEPEND_SRCS)

CLEAN += $(APPLICATION_NAME).elf $(APPLICATION_NAME).hex $(APPLICATION_NAME).ihex $(APPLICATION_NAME).out
CLEAN += symbols.c symbols.h
CLEAN += $(APPLICATION_NAME).d


-include $(patsubst %.o,%.d,$(ALL_OBJS))
# DO NOT DELETE
