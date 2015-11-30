ifeq ($(SDDS_TOPDIR),)
$(error Please set the SDDS_TOPDIR make variable)
endif

ifeq ($(SDDS_OBJDIR),)
$(warning Please set the SDDS_OBJDIR make variable, assuming $(SDDS_TOPDIR)/objs)
SDDS_OBJDIR := $(SDDS_TOPDIR)/objs
endif

ifeq ($(SDDS_PLATFORM),)
$(error Please set the SDDS_PLATFORM make variable)
$(error See the $(SDDS_TOPDIR)/platform directory for possibilities)
endif

ifeq ($(SDDS_ARCH),)
$(error Please set the SDDS_ARCH make variable)
$(error See $(SDDS_TOPDIR)/include/sdds/sdds_types.h for possibilities)
endif

ifeq ($(SDDS_CONSTANTS_FILE),)
$(error Please set SDDS_CONSTANTS_FILE to the location that contains your sDDS constants)
endif

SDDS_SRC_SRCS := $(wildcard $(SDDS_TOPDIR)/src/*.c)
SDDS_SRC_OBJS := $(patsubst $(SDDS_TOPDIR)/src/%.c, \
	$(SDDS_OBJDIR)/sdds-%.o, \
	$(SDDS_SRC_SRCS))

SDDS_PLATFORM_SRCS := $(wildcard $(SDDS_TOPDIR)/platform/$(SDDS_PLATFORM)/*.c)
SDDS_PLATFORM_OBJS := $(patsubst $(SDDS_TOPDIR)/platform/$(SDDS_PLATFORM)/%.c, \
	$(SDDS_OBJDIR)/sdds-$(SDDS_PLATFORM)-%.o, \
	$(SDDS_PLATFORM_SRCS))

#SDDS_DRIVER_SRCS := $(wildcard $(SDDS_TOPDIR)/driver/src/$(SDDS_ARCH)/*.c)
#SDDS_DRIVER_OBJS := $(patsubst $(SDDS_TOPDIR)/driver/src/$(SDDS_ARCH)/%.c, \
	$(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-%.o, \
	$(SDDS_DRIVER_SRCS))

SDDS_OBJS := $(SDDS_SRC_OBJS) $(SDDS_PLATFORM_OBJS) #$(SDDS_DRIVER_OBJS)
SDDS_OBJS_DEPEND := $(patsubst %.o, %.d, $(SDDS_OBJS))

ifeq ($(SDDS_PLATFORM),linux)
CFLAGS += -std=c99
# required for some socket API
CFLAGS += -D_DEFAULT_SOURCE

CFLAGS += -pthread
LDLIBS += -lpthread
endif

ifeq ($(SDDS_PLATFORM),contiki)
# the network module needs that, so pass it to contiki
UIP_CONF_IPV6 := 1

# contiki uses inline assembly
CFLAGS += -std=gnu99

CLEAN += $(SDDS_OBJS) $(SDDS_OBJS_DEPEND)
endif

SDDS_INCLUDES += $(SDDS_TOPDIR)/include/sdds $(SDDS_TOPDIR)/include
SDDS_INCLUDES += $(SDDS_TOPDIR)/platform/$(SDDS_PLATFORM)/include
SDDS_INCLUDES += $(SDDS_TOPDIR)/driver/include
SDDS_INCLUDES += .
AALC_INCLUDE = $(AALC_DIR)/src

CFLAGS_ALL += $(addprefix -I, $(SDDS_INCLUDES))
CFLAGS_ALL += $(addprefix -I, $(AALC_INCLUDE))
CFLAGS_ALL += -DSDDS_PLATFORM_$(SDDS_PLATFORM)
CFLAGS_ALL += -DSDDS_ARCH_$(SDDS_ARCH)
CFLAGS += $(CFLAGS_ALL)
CXXFLAGS += $(CFLAGS_ALL)

$(SDDS_OBJDIR)/sdds-$(SDDS_PLATFORM)-%.o: $(SDDS_TOPDIR)/platform/$(SDDS_PLATFORM)/%.c
	$(COMPILE.c) -MMD $(OUTPUT_OPTION) $<

$(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-%.o: $(SDDS_TOPDIR)/driver/src/$(SDDS_ARCH)/%.c
	$(COMPILE.c) -MMD $(OUTPUT_OPTION) $<

$(SDDS_OBJDIR)/sdds-%.o: $(SDDS_TOPDIR)/src/%.c
	$(COMPILE.c) -MMD $(OUTPUT_OPTION) $<

-include $(SDDS_DEPEND_OBJS_DEPEND)
