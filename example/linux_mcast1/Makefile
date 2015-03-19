all:
	$(MAKE) -f Makefile.include sdds $(MAKEOPTS) \
	            PLATFORM_LINUX_SDDS_SEND_ADDRESS="fe80::10%tap0" \
			    PLATFORM_LINUX_SDDS_PROTOCOL=AF_INET6
clean:
	$(MAKE) -f Makefile.include clean
