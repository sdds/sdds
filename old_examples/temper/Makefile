ifeq ($(SDDS_PLATFORM),)
SDDS_PLATFORM := linux
endif

ifeq ($(SDDS_ARCH),)
SDDS_ARCH := x86
endif

SDDS_TOPDIR := ../..
SDDS_CONSTANTS_FILE := $(SDDS_TOPDIR)/example/temper/gen_constants.h
SDDS_OBJDIR := ./objs

# make sure to set all: if you want to use it, before including sdds.mk

all: $(SDDS_PLATFORM)-app $(SDDS_PLATFORM)-app2

$(SDDS_OBJDIR):
	mkdir $(SDDS_OBJDIR)

$(LOCAL_CONSTANTS):
	touch $(LOCAL_CONSTANTS)

include $(SDDS_TOPDIR)/sdds.mk

$(SDDS_OBJDIR)/%.o: %.c $(SDDS_OBJDIR) $(LOCAL_CONSTANTS) 
	$(COMPILE.c) -MMD $(OUTPUT_OPTION) $<

$(SDDS_OBJDIR)/temper.o: temper.c
	$(COMPILE.c) -MMD $(OUTPUT_OPTION) -D__USE_MISC $<

$(SDDS_PLATFORM)-app: $(SDDS_OBJDIR)/App.o $(SDDS_OBJDIR)/gen_Foo_sDDS.o $(SDDS_OBJS)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

$(SDDS_PLATFORM)-app2: $(SDDS_OBJDIR)/App2.o $(SDDS_OBJDIR)/gen_Foo2_sDDS.o $(SDDS_OBJS)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

$(SDDS_OBJDIR)/test: $(SDDS_OBJDIR)/Test.o $(SDDS_OBJDIR)/gen_Foo_sDDS.o $(SDDS_OBJS)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

clean:
	$(RM) $(SDDS_OBJS) $(SDDS_OBJS_DEPEND)
	$(RM) $(SDDS_PLATFORM)-app $(SDDS_PLATFORM)-app2
	$(RM) $(SDDS_OBJDIR)/App.o $(SDDS_OBJDIR)/App2.o
	$(RM) $(SDDS_OBJDIR)/gen_Foo_sDDS.o $(SDDS_OBJDIR)/gen_Foo2_sDDS.o
	$(RM) $(SDDS_OBJDIR)/bbiic.o $(SDDS_OBJDIR)/temper.o
	$(RM) -rf $(SDDS_OBJDIR)

-include $(SDDS_OBJDIR)/bbiic.d $(SDDS_OBJDIR)/temper.d
