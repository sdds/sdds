all:
	$(MAKE) -f Makefile.include sdds $(MAKEOPTS) \
	            SDDS_LINUX_SEND_ADDRESS="fe80::10%tap0" \
			    SDDS_LINUX_PROTOCOL=AF_INET6
clean:
	$(MAKE) -f Makefile.include clean
