JACKDAW = fe80::12:13ff:fe14:1516
BOARD = fe80::00ff:fe00:0001
PLATFORM_LINUX_SDDS_LISTEN_ADDRESS = "$(JACKDAW)%usb0" 
PLATFORM_LINUX_SDDS_SEND_ADDRESS = "$(BOARD)%usb0"
#CONTIKI = $(shell echo $$HOME)/es-projekt/contiki
CONTIKI = ../../../platform/contiki
TARGET_USE_DERFMEGA128 = 1 

export PLATFORM_LINUX_SDDS_LISTEN_ADDRESS 
export PLATFORM_LINUX_SDDS_SEND_ADDRESS 
export CONTIKI 
export TARGET_USE_DERFMEGA128 

all:
	$(MAKE) -f Makefile_2nd_lvl.mk  CONTIKI=$(CONTIKI) BUILD=sensor_board avr-atmega128rfa1-write.hex $(MAKEOPTS) 
	$(MAKE) -f Makefile_2nd_lvl.mk  BUILD=linux test-write $(MAKEOPTS) 

flash:
	$(MAKE) -f Makefile_2nd_lvl.mk  CONTIKI=$(CONTIKI) BUILD=sensor_board flash $(MAKEOPTS)

debug:
	$(MAKE) -f Makefile_2nd_lvl.mk  CONTIKI=$(CONTIKI) BUILD=sensor_board debug $(MAKEOPTS)

clean:
	$(MAKE) -f Makefile_2nd_lvl.mk  CONTIKI=$(CONTIKI) BUILD=sensor_board clean
	$(MAKE) -f Makefile_2nd_lvl.mk BUILD=linux clean
