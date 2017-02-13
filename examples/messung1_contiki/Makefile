SDDS_TOPDIR := $(shell dirname $(shell dirname $(shell readlink generate.sh)))
CONTIKI = $(SDDS_TOPDIR)/../platform/contiki
BUILD = sensor_board
TARGET_USE_DERFMEGA128 = 1
UIP_CONF_RPL=0

APPLICATION_NAME = messung1_contiki

export APPLICATION_NAME
export CONTIKI
export BUILD
export TARGET_USE_DERFMEGA128
export UIP_CONF_RPL

all:
	$(MAKE) -f Makefile_2nd_lvl.mk $(APPLICATION_NAME).hex $(MAKEOPTS)

flash:
	$(MAKE) -f Makefile_2nd_lvl.mk flash $(MAKEOPTS)

dude:
	$(MAKE) -f Makefile_2nd_lvl.mk dude $(MAKEOPTS)

debug:
	$(MAKE) -f Makefile_2nd_lvl.mk debug $(MAKEOPTS)

gdb-server:
	$(MAKE) -f Makefile_2nd_lvl.mk gdb-server $(MAKEOPTS)

clean:
	$(MAKE) -f Makefile_2nd_lvl.mk clean
