SDDS_TOPDIR := ../..

SDDS_OBJDIR := objs-autobest
TARGET := autobest
SDDS_PLATFORM := autobest
SDDS_ARCH := ARM

DATASTRUCTURES_FILE := datastructures

LOCAL_CONSTANTS := local_constants.h

IMPL_DEPEND_OBJS = $(SDDS_OBJDIR)/sdds_sdds_impl.o
ALL_OBJS += $(IMPL_DEPEND_OBJS)
ALL_OBJS += $(SDDS_OBJDIR)/sdds.o

SDDS_CONSTANTS_FILE := gen_constants.h
#COMPILE.c := $(CROSS)gcc -c
CC = $(CROSS)gcc
LD = $(CROSS)ld

include $(SDDS_TOPDIR)/sdds.mk

INCLUDES += $(AUTOBEST_INCLUDES)

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
CFLAGS += $(INCLUDES)
CFLAGS += -std=c99
CFLAGS += -ffreestanding

CFLAGS += -ggdb3
CFLAGS += $(AUTOBEST_CFLAGS)

LDFLAGS := -r

all:

$(SDDS_OBJDIR):
	mkdir $(SDDS_OBJDIR)

$(LOCAL_CONSTANTS):
	touch $(LOCAL_CONSTANTS)


$(SDDS_OBJDIR)/%.o: %.c
	echo $(SDDS_OBJS) $(IMPL_DEPEND_OBJS) $(DATA_DEPEND_OBJS)
	$(COMPILE.c)  $(CFLAGS) -MMD $(OUTPUT_OPTION) $<

$(APPLICATION_NAME).c: $(LOCAL_CONSTANTS) $(SDDS_OBJDIR) $(IMPL_DEPEND_SRCS) $(DATA_DEPEND_SRCS)

$(APPLICATION_NAME).o: $(SDDS_OBJDIR)/sdds.o $(SDDS_OBJS) $(IMPL_DEPEND_OBJS) $(DATA_DEPEND_OBJS)
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%-ds.c %-ds.h %_sdds_impl.c %_sdds_impl.h:
	$(shell ./generate.sh)

clean:
	$(RM) ./$(APPLICATION_NAME)
	$(RM) $(CLEAN)
	$(RM) $(SDDS_OBJS) $(SDDS_OBJS_DEPEND)
	$(RM) -rf $(SDDS_OBJDIR)

-include $(patsubst %.o,%.d,$(ALL_OBJS))
