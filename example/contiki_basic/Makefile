JACKDAW = fe80::2e5:1aab:cd00:0
PLATFORM_CONTIKI_SDDS_SEND_ADDRESSS = "$(JACKDAW)"
BOARD = fe80::2e5:1aab:cd00:0
CONTIKI = ../../../platform/contiki
BUILD = sensor_board
TARGET_USE_DERFMEGA128 = 1
UIP_CONF_RPL=0

APPLICATION_NAME := mywrite

export APPLICATION_NAME
export CONTIKI
export BUILD
export TARGET_USE_DERFMEGA128
export PLATFORM_CONTIKI_SDDS_SEND_ADDRESSS
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
