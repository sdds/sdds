#JACKDAW = fe80::12:13ff:fe14:1516
JACKDAW = fe80::50:c4ff:fe04:2e3
PLATFORM_CONTIKI_SDDS_SEND_ADDRESSS = "$(JACKDAW)"
BOARD = fe80::50:c4ff:fe04:81a5
#CONTIKI = $(shell echo $$HOME)/es-projekt/contiki
#CONTIKI = ../../../platform/contiki
CONTIKI = ../../../platform/hexabus/firmware/contiki-2.x
BUILD = sensor_board
TARGET_USE_DERFMEGA128 = 1

APPLICATION_NAME := mywrite

export APPLICATION_NAME
export CONTIKI
export BUILD
export TARGET_USE_DERFMEGA128
export PLATFORM_CONTIKI_SDDS_SEND_ADDRESSS

all:
	$(MAKE) -f Makefile_2nd_lvl.mk $(APPLICATION_NAME).hex $(MAKEOPTS)

flash:
	$(MAKE) -f Makefile_2nd_lvl.mk flash $(MAKEOPTS)

debug:
	$(MAKE) -f Makefile_2nd_lvl.mk debug $(MAKEOPTS)

clean:
	$(MAKE) -f Makefile_2nd_lvl.mk clean
# DO NOT DELETE
