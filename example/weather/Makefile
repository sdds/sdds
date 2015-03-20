SDDS_TOPDIR := ../..

ifeq ($(BUILD),linux)

  SDDS_OBJDIR := objs-linux
  TARGET := linux
  SDDS_PLATFORM := linux
  SDDS_ARCH := x86
  
  LDFLAGS += -lm
  LDLIBS += -lm
  
#  CFLAGS += -D_POSIX_C_SOURCE=200809L

  IMPL_DEPEND_OBJS = $(SDDS_OBJDIR)/linux_sdds_impl.o
  ALL_OBJS += $(IMPL_DEPEND_OBJS)
  ALL_OBJS += $(SDDS_OBJDIR)/test-write.o

  ifneq ($(PLATFORM_LINUX_SDDS_PORT),)
    CFLAGS += -DPLATFORM_LINUX_SDDS_PORT=$(PLATFORM_LINUX_SDDS_PORT)
  endif
  ifneq ($(PLATFORM_LINUX_SDDS_PROTOCOL),)
    CFLAGS += -DPLATFORM_LINUX_SDDS_PROTOCOL=$(PLATFORM_LINUX_SDDS_PROTOCOL)
  endif
  ifneq ($(PLATFORM_LINUX_SDDS_LISTEN_ADDRESS),)
    CFLAGS += -DPLATFORM_LINUX_SDDS_LISTEN_ADDRESS=\"$(PLATFORM_LINUX_SDDS_LISTEN_ADDRESS)\"
  endif
  ifneq ($(PLATFORM_LINUX_SDDS_SEND_ADDRESS),)
    CFLAGS += -DPLATFORM_LINUX_SDDS_SEND_ADDRESS=\"$(PLATFORM_LINUX_SDDS_SEND_ADDRESS)\"
  endif

  SDDS_CONSTANTS_FILE := ./linux_constants.h
  DATASTRUCTURES_FILE := datastructures
else

  ifeq ($(BUILD),sensor_board)
    SDDS_OBJDIR := objs-128rfa1
    TARGET := avr-atmega128rfa1
  else
    SDDS_OBJDIR := objs-1284p
    TARGET := avr-raven
  endif

  SDDS_PLATFORM := contiki
  SDDS_ARCH := atmega

  #DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/adc.o
  #DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/spi.o
  #DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/twi.o
  #DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/weather-drivers.o
  DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-adc.o
  DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-spi.o
  DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-twi.o
  DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-anemometer.o
  DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-bmp085.o
  DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-raingauge.o
  DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-sht15.o
  DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-wind_vane.o
  DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-TSL2561.o
  
  
  IMPL_DEPEND_OBJS = $(SDDS_OBJDIR)/atmega_sdds_impl.o
  ALL_OBJS += $(DRIVER_DEPEND_OBJS)
  ALL_OBJS += $(IMPL_DEPEND_OBJS)
  ALL_OBJS += $(SDDS_OBJDIR)/$(TARGET)-write.o

  ifneq ($(PLATFORM_CONTIKI_SDDS_PORT),)
    CFLAGS += -DSDDS_CONTIKIPORT=$(PLATFORM_CONTIKI_SDDS_PORT)
  endif
  ifneq ($(PLATFORM_CONTIKI_SDDS_LISTEN_ADDRESS),)
    CFLAGS += -DPLATFORM_CONTIKI_SDDS_LISTEN_ADDRESS=\"$(PLATFORM_CONTIKI_SDDS_LISTEN_ADDRESS)\"
  endif
  ifneq ($(PLATFORM_CONTIKI_SDDS_SEND_ADDRESSS),)
    CFLAGS += -DPLATFORM_CONTIKI_SDDS_SEND_ADDRESSS=\"$(PLATFORM_CONTIKI_SDDS_SEND_ADDRESSS)\"
  endif

  ifneq ($(TARGET_USE_DERFMEGA128),)
    CFLAGS += -DTARGET_USE_DERFMEGA128
  endif

  SDDS_CONSTANTS_FILE := ./atmega_constants.h
  DATASTRUCTURES_FILE := datastructures
endif

include $(SDDS_TOPDIR)/sdds.mk

ifneq ($(BUILD),linux)
include $(CONTIKI)/Makefile.include
endif

DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/TemperatureAndHumiditySensor-ds.o
DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/AirPressureSensor-ds.o
DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/RainfallSensor-ds.o
DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/TemperatureSensor-ds.o
DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/WindDirectionSensor-ds.o
DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/WindSpeedSensor-ds.o
DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/LightSensor-ds.o
ALL_OBJS += $(DATA_DEPEND_OBJS)

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
	$(shell python $(SDDS_TOPDIR)/generate_sdds.py $(<:-dds-roles=) $(DATASTRUCTURES_FILE))

all:

CFLAGS += -I.

ifeq ($(BUILD),linux)

$(SDDS_OBJDIR)/%.o: %.c
	$(COMPILE.c) $(CFLAGS) -MMD $(OUTPUT_OPTION) $<

test-write.c: $(IMPL_DEPEND_SRCS) $(DATA_DEPEND_SRCS)

test-write: $(SDDS_OBJDIR)/test-write.o $(SDDS_OBJS) $(IMPL_DEPEND_OBJS) $(DATA_DEPEND_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

clean:
	$(RM) ./test-write
	$(RM) $(CLEAN)
	$(RM) $(SDDS_OBJS) $(SDDS_OBJS_DEPEND)

else
$(SDDS_OBJDIR)/%.o: %.c
	$(COMPILE.c) -MMD $(OUTPUT_OPTION) $<

$(TARGET)-write.c: $(IMPL_DEPEND_SRCS) $(DATA_DEPEND_SRCS)
$(TARGET)-write.c: write.c
	$(CC) $(CFLAGS) -MM -MF$(SDDS_OBJDIR)/$(TARGET)-write.d -MT$@ $^
	cp $< $@

$(TARGET)-write.elf: $(TARGET)-write.co $(SDDS_OBJS) $(IMPL_DEPEND_OBJS) $(DATA_DEPEND_OBJS) $(DRIVER_DEPEND_OBJS) contiki-$(TARGET).a
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^ $(LIBC)

%.hex: %.elf
	$(OBJCOPY) $^ -j .text -j .data -O ihex $@

%.ihex: %.elf
	$(OBJCOPY) $^ -O ihex $@

flash:
	avarice -g -e -p -f $(TARGET)-write.hex $(FLASH_ARGS)

debug:
	avr-gdb $(TARGET)-write.elf

CLEAN += $(TARGET)-write.elf $(TARGET)-write.hex $(TARGET)-write.ihex $(TARGET)-write.out
CLEAN += symbols.c symbols.h
CLEAN += $(TARGET)-write.c $(TARGET)-write.d
endif

-include $(patsubst %.o,%.d,$(ALL_OBJS))
