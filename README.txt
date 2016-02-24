.set GIT=https://zenon.cs.hs-rm.de/sdds/sdds.git

# sDDS

<img src="https://zenon.cs.hs-rm.de/uploads/project/avatar/528/Drawing_1_.png" width="200px" height="200px" />

## Contents

.toc 2

## Overview

sDDS is a DDS implementation for embedded device as low as 8-bit microcontrollers.

## Scope and Goals

## Legacy README

NOTE: This information is probably obsolete!

Due to the nature of embedded development kits, this package merely
provides a little help to make it easier to build applications with
the provided SDK but does not itself allow to build it.

This solution was picked because of the following reasons:

- Embedded SDKs usually provide their own set of Makefiles with lots
  of specific flags and macro definitions and provide a small template
  to base applications on.

- No need for modifications on sDDS Makefiles if Makefiles in the SDK
  change.

### BitCloud

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

### Linux

An example that builds on a linux system is provided in example/temper.

### CC2430

TODO

### Contiki

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

## Style Guide

sDDS uses [uCLASS] (https://zenon.cs.hs-rm.de/sdds/sdds/blob/master/style_guide.md) guild for code style.

## sDDS - Code Generator

### Overview

The sDDS Code Generator is responsible for generating DDS topic and DDS participant dependent code. It uses [GSL/4.1](https://github.com/imatix/gsl) as code construction tool.

#### Scope and Goals

The sDDS Code Generator has these primary goals:

* generate topic files
* generate topic implementations
* generate Makefiles
* generate skeletons for new apps

Makefile generation is currently supported for:

* Linux
* Contiki

#### Getting started

To get started with your first app add it to the 'example/examples.xml':

```xml
<examples>
    ...
    <example name = "my_example" os = "myos">My sDDS example</example>
    ...
</examples>
```

Then run the 'example/generate_examples.sh' to create the stub for your app.

### Configuration

#### Topics

Topics are configured in a xml file. By convention these files are placed into 'example/topics' to avoid topic duplication and copy\&paste madness.

```xml
<domain id = "1">
    <topic name = "test1" domain = "1" id = "0">
        Test1 topic
        <attribute name = "val1" type = "DDS_char">My example value 1</attribute>
    </topic>
    <topic name = "test2" domain = "1" id = "1">
        Test2 topic
        <attribute name = "val2" type = "DDS_char">My example value 2</attribute>
    </topic>
</domain>
```

#### Example

To configure your example use the generated sdds.xml and change it to your needs.

```xml
<project
    name = "my_example"
    script = "sdds.gsl"
    endian = "little"
    os = "myos"
    ip = "fe80::12:13ff:fe14:1516"
    iface = "eth0"
    >
    <!--
        Include your topic files here. You can include topics from multiple files.
        Only those topics will be generated that have at least one matching role.
    -->
    <include filename = "../topics/messung.xml" />

    <!--
        Add defines to enable/disable features.
    -->
    <define name = "FEATURE_SDDS_MULTICAST_ENABLED" />
    <define name = "FEATURE_SDDS_BUILTIN_TOPICS_ENABLED" />
    <define name = "FEATURE_SDDS_DISCOVERY_ENABLED" />
    <define name = "SDDS_NET_MAX_BUF_SIZE" value = "1024"/>
    <define name = "SDDS_NET_MAX_LOCATOR_COUNT" value = "20"/>
    <define name = "SDDS_DISCOVERY_RECEIVE_TIMER" value = "1"/>
    <define name = "SDDS_DISCOVERY_PARTICIPANT_TIMER" value = "1"/>
    <define name = "SDDS_DISCOVERY_PUBLICATION_TIMER" value = "5"/>
    <define name = "SDDS_DISCOVERY_TOPIC_TIMER" value = "0"/>
    <define name = "SDDS_QOS_LATENCYBUDGET"/>
    <define name = "SDDS_QOS_DW_LATBUD" value = "5000"/>
    <define name = "SDDS_QOS_DW_COMM" value = "1000"/>
    <define name = "SDDS_QOS_DW_READ" value = "1000"/>

    <!--
        Roles are bound to one topic and are either a publisher or subscriber.
        Publisher can have a list of subscriber to whom the should send data.
        If there's no subscriber for a publisher sDDS will automatically use
        discovery.
    -->
    <role topic = "alpha" type = "publisher">
        <subscriber ip = "fe80::12:13ff:fe14:1516" />
    </role>
</project>
```

Once you're done configuring your example run 'generate.sh' to create the Makefile after that you can you 'make' and 'make clean' to build your example.

NOTE: The first time you're running 'generate.sh' it will download and install the code generator which will take some time!

### Code Generator

The code generator is split into two distinct generation processes:

#### Generate an app stub

[diagram]
    +---------------------------+           +---------------------+
    | scripts/sdds_examples.gsl |   Input   | example.xml         |
    +---------------------------+<----------+---------------------+
    | Script to generate app    |           | Defines all example |
    | stub.                 cFDA|           | apps            cFDA|
    +-------------+-------------+           +---------------------+
                  |
                  |                         +---------------------+
                  |       Output            | example/your_app/   |
                  +------------------------>+---------------------+
                                            | * sdds.xml      cFAD|
                                            +---------------------+
[/diagram]

This is done by 'sdds_examples.gsl'.

#### Generate example app

The entry point is 'sdds.gsl':

[diagram]
    +---------------------------+           +---------------------+
    | scripts/sdds.gsl          |   Input   | sdds.xml            |
    +---------------------------+<----------+---------------------+
    | Script to generate sdds   |           | Describes you app's |
    | constants, topics,    cFDA|           | configuration and   |
    | skeletion and glue code.  |           | features.       cFDA|
    +-------------+-------------+           +---------------------+
                  |
                  |                         +--------------------------+
                  |       Output            | example/your_app/        |
                  +------------------------>+--------------------------+
                                            | * <your_app>_topic.(c|h) |
                                            | * <your_app>_impl.(c|h)  |
                                            | * gen_constants.h        |
                                            | * local_constants.h      |
                                            | * <your_app>.c           |
                                            | * Makefile               |
                                            | * Makefile_2nd_lvl   cFAD|
                                            +--------------------------+
[/diagram]

* sdds_topic.gsl - Generate the topics
* sdds_impl.gsl - Generate the implementation of the topics
* sdds_constants.gsl - Generate the default constants for sDDS
* sdds_make\_<OS>.gsl - Generates the Makefiles for the different operating systems
* sdds_skeleton - Generates a skeleton for new examples dependent on topics and roles

## Acknowledgements

sDDS Logo Copyright CC BY 3.0 - Scheme by Flaticon from Freepik

### This Document

This document is originally at README.txt and is built using [gitdown](http://github.com/imatix/gitdown).
