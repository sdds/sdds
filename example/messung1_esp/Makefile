APPLICATION_NAME := sdds
PLATTFORM_RTOS_IP_PROTOCOL := ipv4

export APPLICATION_NAME
export PLATTFORM_RTOS_IP_PROTOCOL

all:
	$(MAKE) -f Makefile_2nd_lvl.mk $(APPLICATION_NAME).hex $(MAKEOPTS)

flash:
	$(MAKE) -f Makefile_2nd_lvl.mk flash $(MAKEOPTS)

dude:
	$(MAKE) -f Makefile_2nd_lvl.mk dude $(MAKEOPTS)

debug:
	$(MAKE) -f Makefile_2nd_lvl.mk debug $(MAKEOPTS)

clean:
	$(MAKE) -f Makefile_2nd_lvl.mk clean
