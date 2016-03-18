# name of your application
APPLICATION = default

# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-xpro

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../platform/riot/

SDDS_TOPDIR ?= $(CURDIR)/../../
SDDS_OBJDIR ?= $(CURDIR)/obj
SDDS_PLATFORM ?= riot
SDDS_ARCH ?= ARM
SDDS_CONSTANTS_FILE ?= $(CURDIR)/gen_constants.h
LOCAL_CONSTANTS ?= $(CURDIR)/local_constants.h
SDDS_APPDIR ?= $(CURDIR)

export SDDS_TOPDIR
export SDDS_OBJDIR
export SDDS_PLATFORM
export SDDS_ARCH
export SDDS_CONSTANTS_FILE
export SDDS_APPDIR
export RIOTBASE
export BOARD

# Uncomment these lines if you want to use platform support from external
# repositories:
#RIOTCPU ?= $(CURDIR)/../../RIOT/thirdparty_cpu
#RIOTBOARD ?= $(CURDIR)/../../RIOT/thirdparty_boards

# Uncomment this to enable scheduler statistics for ps:
#CFLAGS += -DSCHEDSTATISTICS

# If you want to use native with valgrind, you should recompile native
# with the target all-valgrind instead of all:
# make -B clean all-valgrind

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 0

# Modules to include:

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += vtimer
USEMODULE += config
USEMODULE += gnrc_netif_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for IPv6 and UDP
USEMODULE += gnrc_ipv6
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_udp
# Add a routing protocol
USEMODULE += gnrc_rpl
USEMODULE += gnrc_conn_udp
USEMODULE += gnrc_icmpv6_echo

USEMODULE += gnrc_ndp
# Additional networking modules that can be dropped if not needed
# Use minimal standard PRNG
USEMODULE += prng_minstd

CFLAGS += -DGNRC_IPV6_NETIF_ADDR_NUMOF=12


FEATURES_OPTIONAL += transceiver
FEATURES_OPTIONAL += config
FEATURES_OPTIONAL += periph_rtc

ifneq (,$(filter msb-430,$(BOARD)))
	USEMODULE += sht11
endif
ifneq (,$(filter msba2,$(BOARD)))
	USEMODULE += sht11
#	USEMODULE += ltc4150
	USEMODULE += mci
	USEMODULE += random
endif
ifneq (,$(filter native,$(BOARD)))
	USEMODULE += ltc4150
	USEMODULE += random
endif
ifneq (,$(filter iot-lab_M3,$(BOARD)))
	USEMODULE += isl29020
	USEMODULE += lps331ap
	USEMODULE += l3g4200d
	USEMODULE += lsm303dlhc
endif
ifneq (,$(filter fox,$(BOARD)))
        USEMODULE += lps331ap
        USEMODULE += l3g4200d
	USEMODULE += lsm303dlhc
endif

include ../../sdds.mk

# depended/ generated objects
DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/ipc-ds.o
IMPL_DEPEND_OBJS = $(SDDS_OBJDIR)/riot_sdds_impl.o

# driver
# DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-LED.o

%-ds.c %-ds.h %_sdds_impl.c %_sdds_impl.h:
	$(shell ./generate.sh)

#SDDS_OBJS += $(DATA_DEPEND_OBJS)
#SDDS_OBJS += $(IMPL_DEPEND_OBJS)

DATA_DEPEND_SRCS += $(patsubst $(SDDS_OBJDIR)/%.o,%.c,$(DATA_DEPEND_OBJS))
DATA_DEPEND_SRCS += $(patsubst $(SDDS_OBJDIR)/%.o,%.h,$(DATA_DEPEND_OBJS))
IMPL_DEPEND_SRCS += $(patsubst $(SDDS_OBJDIR)/%.o,%.c,$(IMPL_DEPEND_OBJS))
IMPL_DEPEND_SRCS += $(patsubst $(SDDS_OBJDIR)/%.o,%.h,$(IMPL_DEPEND_OBJS))

$(SDDS_OBJS): $(IMPL_DEPEND_SRCS) $(DATA_DEPEND_SRCS) $(SDDS_CONSTANTS_FILE) $(LOCAL_CONSTANTS) $(SDDS_OBJDIR)

foo:
	echo $(IMPL_DEPEND_SRCS) $(DATA_DEPEND_SRCS)

$(LOCAL_CONSTANTS):
	touch $(LOCAL_CONSTANTS)

$(SDDS_OBJDIR):
	mkdir $(SDDS_OBJDIR)

clean2:
	rm $(DATA_DEPEND_SRCS)
	rm $(IMPL_DEPEND_SRCS)
	rm $(SDDS_CONSTANTS_FILE)
	rm $(LOCAL_CONSTANTS)
	rm $(SDDS_OBJDIR)/*


#DIRS += $(SDDS_TOPDIR)
INCLUDES += -I $(RIOTBASE)/core/include/
INCLUDES += -I $(RIOTBASE)/sys/include/
CFLAGS += $(INCLUDES)
CFLAGS += -DTHREAD_STACKSIZE_DEFAULT=1024

APPDEPS += $(SDDS_OBJS)
include $(RIOTBASE)/Makefile.include
