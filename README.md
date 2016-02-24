
<A name="toc1-2" title="sDDS" />
# sDDS

<center>
<img src="https://zenon.cs.hs-rm.de/sdds/sdds/raw/master/images/README_1.png" alt="1">
</center>

<A name="toc2-22" title="Contents" />
## Contents


**<a href="#toc2-27">Overview</a>**

**<a href="#toc2-32">Scope and Goals</a>**

**<a href="#toc2-35">Legacy README</a>**
*  <a href="#toc3-53">BitCloud</a>
*  <a href="#toc3-141">Linux</a>
*  <a href="#toc3-146">CC2430</a>
*  <a href="#toc3-151">Contiki</a>

**<a href="#toc2-196">Style Guide</a>**

**<a href="#toc2-201">sDDS - Code Generator</a>**
*  <a href="#toc3-204">Overview</a>
&emsp;<a href="#toc4-209">Scope and Goals</a>
&emsp;<a href="#toc4-224">Getting started</a>
*  <a href="#toc3-239">Configuration</a>
&emsp;<a href="#toc4-242">Topics</a>
&emsp;<a href="#toc4-260">Example</a>
*  <a href="#toc3-313">Code Generator</a>
&emsp;<a href="#toc4-318">Generate an app stub</a>
&emsp;<a href="#toc4-338">Generate example app</a>
*  <a href="#toc3-371">This Document</a>

<A name="toc2-27" title="Overview" />
## Overview

sDDS is a DDS implementation for embedded device as low as 8-bit microcontrollers.

<A name="toc2-32" title="Scope and Goals" />
## Scope and Goals

<A name="toc2-35" title="Legacy README" />
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

<A name="toc3-53" title="BitCloud" />
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

<A name="toc3-141" title="Linux" />
### Linux

An example that builds on a linux system is provided in example/temper.

<A name="toc3-146" title="CC2430" />
### CC2430

TODO

<A name="toc3-151" title="Contiki" />
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

<A name="toc2-196" title="Style Guide" />
## Style Guide

sDDS uses [uCLASS] (https://zenon.cs.hs-rm.de/sdds/sdds/blob/master/style_guide.md) guild for code style.

<A name="toc2-201" title="sDDS - Code Generator" />
## sDDS - Code Generator

<A name="toc3-204" title="Overview" />
### Overview

The sDDS Code Generator is responsible for generating DDS topic and DDS participant dependent code. It uses [GSL/4.1](https://github.com/imatix/gsl) as code construction tool.

<A name="toc4-209" title="Scope and Goals" />
#### Scope and Goals

The sDDS Code Generator has these primary goals:

* generate topic files
* generate topic implementations
* generate Makefiles
* generate skeletons for new apps

Makefile generation is currently supported for:

* Linux
* Contiki

<A name="toc4-224" title="Getting started" />
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

<A name="toc3-239" title="Configuration" />
### Configuration

<A name="toc4-242" title="Topics" />
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

<A name="toc4-260" title="Example" />
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

<A name="toc3-313" title="Code Generator" />
### Code Generator

The code generator is split into two distinct generation processes:

<A name="toc4-318" title="Generate an app stub" />
#### Generate an app stub

<center>
<img src="https://zenon.cs.hs-rm.de/sdds/sdds/raw/master/images/README_2.png" alt="2">
</center>

This is done by 'sdds_examples.gsl'.

<A name="toc4-338" title="Generate example app" />
#### Generate example app

The entry point is 'sdds.gsl':

<center>
<img src="https://zenon.cs.hs-rm.de/sdds/sdds/raw/master/images/README_3.png" alt="3">
</center>

* sdds_topic.gsl - Generate the topics
* sdds_impl.gsl - Generate the implementation of the topics
* sdds_constants.gsl - Generate the default constants for sDDS
* sdds_make\_<OS>.gsl - Generates the Makefiles for the different operating systems
* sdds_skeleton - Generates a skeleton for new examples dependent on topics and roles

<A name="toc3-371" title="This Document" />
### This Document

_This documentation was generated using [Gitdown](https://github.com/zeromq/gitdown)_
