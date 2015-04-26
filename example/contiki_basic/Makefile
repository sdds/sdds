JACKDAW = fe80::12:13ff:fe14:1516
PLATFORM_CONTIKI_SDDS_SEND_ADDRESSS = "$(JACKDAW)"
BOARD = fe80::00ff:fe00:0001
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

debug:
	$(MAKE) -f Makefile_2nd_lvl.mk debug $(MAKEOPTS)

clean:
	$(MAKE) -f Makefile_2nd_lvl.mk clean
