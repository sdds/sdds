SDDS_TOPDIR := ../../..

SDDS_OBJDIR := objs-linux
TARGET := linux
SDDS_PLATFORM := linux
SDDS_ARCH := x86

IMPL_DEPEND_OBJS = $(SDDS_OBJDIR)/linux_sdds_impl.o
ALL_OBJS += $(IMPL_DEPEND_OBJS)
ALL_OBJS += $(SDDS_OBJDIR)/sdds.o

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

SDDS_CONSTANTS_FILE := ./linux_constants.h

include $(SDDS_TOPDIR)/sdds.mk

DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/ipc-ds.o
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

%-ds.c %-ds.h: datastructures
	$(shell python $(SDDS_TOPDIR)/generate_ds.py $<)

%_sdds_impl.c %_sdds_impl.h: %-dds-roles datastructures $(DATA_DEPEND_SRCS)
	$(shell python $(SDDS_TOPDIR)/generate_sdds.py $(<:-dds-roles=))

all:

CFLAGS += -I.
CFLAGS += -O0 -ggdb3

$(SDDS_OBJDIR)/%.o: %.cc
	echo $(SDDS_DEPEND_OBJS) $(IMPL_DEPEND_OBJS) $(DATA_DEPEND_OBJS)
	$(COMPILE.cc) -std=c++0x $(CFLAGS) -MMD $(OUTPUT_OPTION) $<

$(SDDS_OBJDIR)/%.o: %.c
	$(COMPILE.c) $(CFLAGS) -MMD $(OUTPUT_OPTION) $<

sdds.cc: $(IMPL_DEPEND_SRCS) $(DATA_DEPEND_SRCS)

sdds: $(SDDS_OBJDIR)/sdds.o $(SDDS_DEPEND_OBJS) $(IMPL_DEPEND_OBJS) $(DATA_DEPEND_OBJS)
	$(CXX) -std=c++0x -o $@ $^ $(LDLIBS)

clean:
	$(RM) ./sdds
	$(RM) $(CLEAN)
	$(RM) $(SDDS_DEPEND_OBJS) $(SDDS_DEPEND_OBJS_DEPEND)

-include $(patsubst %.o,%.d,$(ALL_OBJS))
