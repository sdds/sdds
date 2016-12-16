
APPLICATION_NAME = sdds_app2

export APPLICATION_NAME

ifeq ($(AUTOBEST_INCLUDES),)
$(error Please set the AUTOBEST_INCLUDES make variable)
endif
export AUTOBEST_INCLUDES
ifeq ($(CROSS),)
$(error Please set the CROSS make variable)
endif
export CROSS
ifeq ($(AUTOBEST_CFLAGS),)
$(error Please set the AUTOBEST_CFLAGS make variable)
endif
export AUTOBEST_CFLAGS
ifeq ($(DEBUG),)
$(error Please set the DEBUG make variable)
endif
export DEBUG

all:
	$(MAKE) -f Makefile_2nd_lvl.mk   $(MAKEOPTS) lib$(APPLICATION_NAME).a

clean:
	$(MAKE) -f Makefile_2nd_lvl.mk clean
