SDDS_TOPDIR := ../..

SDDS_OBJDIR := objs-linux
TARGET := linux
SDDS_PLATFORM := linux
SDDS_ARCH := x86
  
APPLICATION_NAME := mywrite

DATASTRUCTURES_FILE := datastructures

# Object files of the generateted dds data types
#DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/power_cur-ds.o
DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/test-ds.o

# object files depending on driver for sensors
#  SENSORIO_DEPEND_OBJS += $(SDDS_OBJDIR)/weather-drivers.o

# object files of the generates implementation code file of sdds
IMPL_DEPEND_OBJS = $(SDDS_OBJDIR)/linux_sdds_impl.o

# file for the preprocessor constants of sdds
SDDS_CONSTANTS_FILE := ./linux_constants.h


ALL_OBJS += $(SENSORIO_DEPEND_OBJS)
ALL_OBJS += $(IMPL_DEPEND_OBJS)
ALL_OBJS += $(SDDS_OBJDIR)/$(APPLICATION_NAME).o
ALL_OBJS += $(DATA_DEPEND_OBJS)
  
LDFLAGS += -lm
  
  
  
#  CFLAGS += -D_POSIX_C_SOURCE=200809L


ifneq ($(SDDS_LINUX_PORT),)
  CFLAGS += -DSDDS_LINUX_PORT=$(SDDS_LINUX_PORT)
endif
ifneq ($(SDDS_LINUX_PROTOCOL),)
  CFLAGS += -DSDDS_LINUX_PROTOCOL=$(SDDS_LINUX_PROTOCOL)
endif
ifneq ($(SDDS_LINUX_LISTEN_ADDRESS),)
  CFLAGS += -DSDDS_LINUX_LISTEN_ADDRESS=\"$(SDDS_LINUX_LISTEN_ADDRESS)\"
endif
ifneq ($(SDDS_LINUX_SEND_ADDRESS),)
  CFLAGS += -DSDDS_LINUX_SEND_ADDRESS=\"$(SDDS_LINUX_SEND_ADDRESS)\"
endif


include $(SDDS_TOPDIR)/sdds.mk


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
	$(COMPILE.c) $(CFLAGS) -MMD $(OUTPUT_OPTION) $<

$(APPLICATION_NAME).c: $(IMPL_DEPEND_SRCS) $(DATA_DEPEND_SRCS)

$(APPLICATION_NAME): $(SDDS_OBJDIR)/$(APPLICATION_NAME).o $(SDDS_OBJS) $(IMPL_DEPEND_OBJS) $(DATA_DEPEND_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

clean:
	$(RM) ./$(APPLICATION_NAME)
	$(RM) $(CLEAN)
	$(RM) $(SDDS_OBJS) $(SDDS_OBJS_DEPEND)


-include $(patsubst %.o,%.d,$(ALL_OBJS))
