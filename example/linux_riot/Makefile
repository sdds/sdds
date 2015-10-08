PLATFORM_RIOT_SDDS_LISTEN_ADDRESS=fe80::ff:fe00:59ef
PLATFORM_RIOT_SDDS_SEND_ADDRESS=fe80::ff:fe00:1bef

PLATFORM_RIOT_SDDS_PROTOCOL=AF_INET6

RIOT = ../../../platform/riot
PORT=tap0
USER_CPU_ID=2000
APPLICATION_NAME = sdds

export APPLICATION_NAME
export RIOT
export PORT
export USER_CPU_ID
export PLATFORM_RIOT_SDDS_LISTEN_ADDRESS
export PLATFORM_RIOT_SDDS_SEND_ADDRESS
export PLATFORM_RIOT_SDDS_PROTOCOL

all:
	$(MAKE) -f Makefile_2nd_lvl.mk  $(MAKEOPTS)

clean:
	$(MAKE) -f Makefile_2nd_lvl.mk cleanlv2

term:
	$(MAKE) -f Makefile_2nd_lvl.mk term

flash: 
	$(MAKE) -f Makefile_2nd_lvl.mk flash