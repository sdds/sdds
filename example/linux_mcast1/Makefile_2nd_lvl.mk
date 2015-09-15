SDDS_TOPDIR := ../..

SDDS_OBJDIR := objs-linux
TARGET := linux
SDDS_PLATFORM := linux
SDDS_ARCH := x86

DATASTRUCTURES_FILE := datastructures

LOCAL_CONSTANTS := local_constants.h

IMPL_DEPEND_OBJS = $(SDDS_OBJDIR)/linux_sdds_impl.o
ALL_OBJS += $(IMPL_DEPEND_OBJS)
ALL_OBJS += $(SDDS_OBJDIR)/sdds.o

SDDS_CONSTANTS_FILE := gen_constants.h

include $(SDDS_TOPDIR)/sdds.mk

DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/beta-ds.o
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

$(SDDS_OBJDIR):
	mkdir $(SDDS_OBJDIR)

$(LOCAL_CONSTANTS):
	touch $(LOCAL_CONSTANTS)
	
$(SDDS_CONSTANTS_FILE):
	touch $(SDDS_CONSTANTS_FILE)

CFLAGS += -I.
CFLAGS += -O0 -ggdb3
LDLIBS += -lpthread

$(SDDS_OBJDIR)/%.o: %.c
	echo $(SDDS_OBJS) $(IMPL_DEPEND_OBJS) $(DATA_DEPEND_OBJS)
	$(COMPILE.c)  $(CFLAGS) -MMD $(OUTPUT_OPTION) $<

$(SDDS_OBJDIR)/%.o: %.c
	$(COMPILE.c) $(CFLAGS) -MMD $(OUTPUT_OPTION) $<

$(APPLICATION_NAME).c: $(LOCAL_CONSTANTS) $(SDDS_OBJDIR) $(IMPL_DEPEND_SRCS) $(DATA_DEPEND_SRCS) $(SDDS_CONSTANTS_FILE)

$(APPLICATION_NAME): $(SDDS_OBJDIR)/sdds.o $(SDDS_OBJS) $(IMPL_DEPEND_OBJS) $(DATA_DEPEND_OBJS)
	$(CC) -o $@ $^ $(LDLIBS)

clean:
	$(RM) ./$(APPLICATION_NAME)
	$(RM) $(CLEAN)
	$(RM) $(SDDS_OBJS) $(SDDS_OBJS_DEPEND)
	$(RM) -rf $(SDDS_OBJDIR)

-include $(patsubst %.o,%.d,$(ALL_OBJS))
