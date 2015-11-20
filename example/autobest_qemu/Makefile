
APPLICATION_NAME = sdds

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

all:
	$(MAKE) -f Makefile_2nd_lvl.mk  $(MAKEOPTS) $(APPLICATION_NAME).o

clean:
	$(MAKE) -f Makefile_2nd_lvl.mk clean
