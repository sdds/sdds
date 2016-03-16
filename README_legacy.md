# README

Due to the nature of embedded development kits, this package merely
provides a little help to make it easier to build applications with
the provided SDK but does not itself allow to build it.

This solution was picked because of the following reasons:

- Embedded SDKs usually provide their own set of Makefiles with lots
  of specific flags and macro definitions and provide a small template
  to base applications on.

- No need for modifications on sDDS Makefiles if Makefiles in the SDK
  change.

## BitCloud

The top level directory of the BitCloud SDK is refered to as TOPDIR
during this rough description (at the time this document was written
TOPDIR was identical to BitCloud_ATAVRRZRAVEN_1_11_0).

Take a look at the examples in `TOPDIR/Applications`.

All you have to do is to create an Application based on that structure
and modify the relevant target Makefile a tiny bit.

Consider the example in BitCloud 1.11.0 in `TOPDIR/Applications/Blink`.

The relevant target Makefiles are located in
`TOPDIR/Applications/Blink/makefiles`.

Consider the Makefile:
`TOPDIR/Applications/Blink/makefiles/UsbDongle/Makefile_All_UsbDongle_At90usb1287_Rf230B_8Mhz_Gcc`

Switch to the application dir and open the Makefile.
At the top you will see some project specific settings like APP_NAME.
Adjust them for your needs.
Once you're done, check that the Makefile includes the Makerules file like so:
`include ./../../../../BitCloud/lib/Makerules_At90usb1287_Gcc`

In between those definitions and the inclusion of the Makerules, you should
add the following lines to build a project with sDDS support:

```
    SDDS_ARCH := ATmega
    SDDS_PLATFORM := BitCloud
    SDDS_OBJDIR := $(OBJ_PATH)
    SDDS_CONSTANTS_FILE := ../../atmega_constants.h
    SDDS_TOPDIR := $(shell echo $$HOME/path/to/sdds)
```

For information about those variables, look into the file `sdds.mk`.

If you want to overwrite the suggested compiler by BitCloud, you have to set
the expected version to the one you have installed on your system. To do so,
add the following to the edited Makefile.

```
    EXP_COMPILER := $(shell $(CC) -v 2>&1 | tail -n 1)
```

After the inclusion of the Makerules, you'll have to include the build rules
for sDDS by using:

```
    include $(SDDS_TOPDIR)/sdds.mk
```

The order is important, because Makerules initially set the CFLAGS,
overwriting the sDDS CFLAGS.

Search the line where the linker is invoked, so your sDDS object files get
linked into the resulting binary.

In the example, at the time of writing, the specific build rule was:

```
    $(EXE_PATH)/$(APP_NAME).elf: $(OBJS)
    $(LD) $(LINKER_FLAGS) $(OBJS) -Wl,-\( $(LIBS) -Wl,-\) -o $@

Edit those lines to include the sDDS object files, so it'll look like:

    $(EXE_PATH)/$(APP_NAME).elf: $(OBJS) $(SDDS_OBJS)
    $(LD) $(LINKER_FLAGS) $(OBJS) $(SDDS_OBJS) -Wl,-\( $(LIBS) -Wl,-\) -o $@
```

Almost done.
In order to clean up those objects, you'll also have to edit the Makefile where the
clean target is specified.

```
clean:
    rm -rf $(CONFIG_NAME) ./../../$(APP_NAME).elf ./../../$(APP_NAME).hex ./../../$(APP_NAME).srec ./../../$(APP_NAME).bin

    Simply add the line:

    $(RM) $(SDDS_OBJS) $(SDDS_OBJS_DEPEND)
```

Now you can modify the INCLUDES, LIBS and SRCS make variables in the edited Makefile
for your needs and execute make all to build your application.

## Linux

An example that builds on a linux system is provided in example/temper.

## CC2430

TODO

## Contiki

There's an example in examples/weather.

The building procedure somewhat borrows the ideas from the BitCloud procedure
above. Look into the building instructions for Contiki for an overview.

Create a new application based on some application in the examples
subdirectory of contiki. Create a new Makefile with the following contents.

```
    SDDS_ARCH := ATmega
    SDDS_PLATFORM := contiki
    SDDS_OBJDIR := obj_avr-raven
    SDDS_CONSTANTS_FILE := ./atmega_constants.h
    SDDS_TOPDIR := $(shell echo $$HOME/path/to/sdds)
    CONTIKI := $(shell echo $$HOME/path/to/contiki)
    TARGET := avr-raven

    include $(SDDS_TOPDIR)/sdds.mk
    include $(CONTIKI)/Makefile.include
```

For information about those variables, look into the file `sdds.mk`.

For information about CONTIKI and TARGET, check the contiki
documentation.

All that's left is to add dependencies for the final binary and custom build
ruiles if needed.

```
    sdds-example.elf: $(SDDS_OBJS)

    CLEAN += sdds-example.elf

    Invoke `make sdds-example.elf`.
```

This will build sdds-example.elf from sdds-example.c and link all the sDDS
object files to the resulting binary. Fiddle with the Makefile as you like,
e.g. by creating an all target or use the contiki APPS features, to build
common objects and use common dependencies.

