SDDS_TOPDIR := ../..

SDDS_OBJDIR := objs-linux
TARGET := linux
SDDS_PLATFORM := linux
SDDS_ARCH := x86

LOCAL_CONSTANTS := local_constants.h

IMPL_DEPEND_OBJS = $(SDDS_OBJDIR)/messung2_linux_sdds_impl.o
ALL_OBJS += $(IMPL_DEPEND_OBJS)
ALL_OBJS += $(SDDS_OBJDIR)/messung2_linux.o

SDDS_CONSTANTS_FILE := ./gen_constants.h

include $(SDDS_TOPDIR)/sdds.mk

DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/alpha-ds.o
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

all:

$(SDDS_OBJDIR):
	mkdir $(SDDS_OBJDIR)

$(LOCAL_CONSTANTS):
	touch $(LOCAL_CONSTANTS)

CFLAGS += -I.
CFLAGS += -O0 -ggdb3
LDLIBS += -lpthread

$(SDDS_OBJDIR)/%.o: %.c
	echo $(SDDS_OBJS) $(IMPL_DEPEND_OBJS) $(DATA_DEPEND_OBJS)
	$(COMPILE.c)   $(CFLAGS) -MMD $(OUTPUT_OPTION) $<

$(SDDS_OBJDIR)/%.o: %.c
	$(COMPILE.c) $(CFLAGS) -MMD $(OUTPUT_OPTION) $<

$(APPLICATION_NAME).c: $(LOCAL_CONSTANTS) $(SDDS_OBJDIR) $(IMPL_DEPEND_SRCS) $(DATA_DEPEND_SRCS)

$(APPLICATION_NAME): $(SDDS_OBJDIR)/messung2_linux.o $(SDDS_OBJS) $(IMPL_DEPEND_OBJS) $(DATA_DEPEND_OBJS)
	$(CC) -o $@ $^ $(LDLIBS)

%-ds.c %-ds.h %_sdds_impl.c %_sdds_impl.h:
	$(shell ./generate.sh)

code:
	$(shell ./generate.sh)

clean:
	$(RM) ./$(APPLICATION_NAME)
	$(RM) $(CLEAN)
	$(RM) $(SDDS_OBJS) $(SDDS_OBJS_DEPEND)
	$(RM) -rf $(SDDS_OBJDIR)

-include $(patsubst %.o,%.d,$(ALL_OBJS))
