APPLICATION_NAME := mywrite
PLATTFORM_RTOS_IP_PROTOCOL := ipv4

export APPLICATION_NAME
export PLATTFORM_RTOS_IP_PROTOCOL

all: gen
	$(MAKE) -f Makefile_2nd_lvl.mk $(APPLICATION_NAME).hex $(MAKEOPTS)
	
gen:
	$(MAKE) -f Makefile_2nd_lvl.mk gen
			    
flash:
	$(MAKE) -f Makefile_2nd_lvl.mk flash $(MAKEOPTS)

dude:
	$(MAKE) -f Makefile_2nd_lvl.mk dude $(MAKEOPTS)

debug:
	$(MAKE) -f Makefile_2nd_lvl.mk debug $(MAKEOPTS)

clean:
	$(MAKE) -f Makefile_2nd_lvl.mk clean
