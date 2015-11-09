SDDS_TOPDIR := ../../

DRIVER := $(SDDS_TOPDIR)/include/

ifeq ($(BUILD),sensor_board)
  SDDS_OBJDIR := objs-128rfa1
  TARGET := avr-atmega128rfa1
else
  SDDS_OBJDIR := objs-1284p
  TARGET := avr-raven
endif

SDDS_PLATFORM := contiki
SDDS_ARCH := atmega

MODULES += core/net/ipv6/multicast

CONTIKI_WITH_IPV6 = 1

LOCAL_CONSTANTS := local_constants.h

# Object files of the generateted dds data types
DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/alpha-ds.o

#OBJS = $($(shell ls *-ds.c):.o=.c)
#DATA_DEPEND_OBJS += $(addprefix $(SDDS_OBJDIR)/, $(OBJS))

# object files depending on platform
#PLATFORM_DEPEND_OBJS += $(SDDS_OBJDIR)/

# object files depending on driver for sensors
#DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-LED.o

# object files of the generates implementation code file of sdds
IMPL_DEPEND_OBJS = $(SDDS_OBJDIR)/messung2_contiki_sdds_impl.o

# file for the preprocessor constants of sdds
SDDS_CONSTANTS_FILE := ./gen_constants.h

#ALL_OBJS += $(PLATFORM_DEPEND_OBJS)
ALL_OBJS += $(DRIVER_DEPEND_OBJS)
ALL_OBJS += $(IMPL_DEPEND_OBJS)
ALL_OBJS += $(SDDS_OBJDIR)/$(APPLICATION_NAME).o
ALL_OBJS += $(DATA_DEPEND_OBJS)

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

all:

$(SDDS_OBJDIR):
	mkdir $(SDDS_OBJDIR)

$(LOCAL_CONSTANTS):
	touch $(LOCAL_CONSTANTS)

CFLAGS += -I.
CFLAGS += -I $(DRIVER)
CFLAGS += -Os

$(SDDS_OBJDIR)/%.o: %.c
	$(COMPILE.c) -MMD $(OUTPUT_OPTION) $<

$(APPLICATION_NAME).c: $(LOCAL_CONSTANTS) $(SDDS_OBJDIR) $(APPLICATION_NAME).c $(IMPL_DEPEND_SRCS) $(DATA_DEPEND_SRCS)
	$(CC) $(CFLAGS) -MM -MF$(SDDS_OBJDIR)/$(APPLICATION_NAME).d -MT$@ $^


$(APPLICATION_NAME).elf: $(APPLICATION_NAME).co $(SDDS_OBJS) $(IMPL_DEPEND_OBJS) $(DATA_DEPEND_OBJS) $(DRIVER_DEPEND_OBJS) contiki-$(TARGET).a
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^ $(LIBC)

%.hex: %.elf
	$(OBJCOPY) $^ -j .text -j .data -O ihex $@

%.ihex: %.elf
	$(OBJCOPY) $^ -O ihex $@

flash:
	avarice -g -e -p -f $(APPLICATION_NAME).hex $(FLASH_ARGS)

dude:
	sudo avrdude -c dragon_jtag -p m128rfa1 -U flash:w:$(APPLICATION_NAME).hex

gdb-server:
	sudo avarice -g -j usb -P atmega128rfa1 :4242

debug:
	ddd --debugger "avr-gdb" $(APPLICATION_NAME).elf

CLEAN += $(APPLICATION_NAME).elf $(APPLICATION_NAME).hex $(APPLICATION_NAME).ihex $(APPLICATION_NAME).out
CLEAN += symbols.c symbols.h
CLEAN += $(APPLICATION_NAME).d
CLEAN += -rf $(SDDS_OBJDIR)


-include $(patsubst %.o,%.d,$(ALL_OBJS))

code:
	$(shell ./generate.sh)
