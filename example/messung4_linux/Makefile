APPLICATION_NAME = sdds

export APPLICATION_NAME

all:
	$(MAKE) -f Makefile_2nd_lvl.mk $(APPLICATION_NAME) $(MAKEOPTS)

clean:
	$(MAKE) -f Makefile_2nd_lvl.mk clean

messung: all
	strip objs-linux/*.o
	size objs-linux/*.o > Messung4_Linux.csv
