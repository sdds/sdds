<center>
<img src="https://zenon.cs.hs-rm.de/sdds/sdds/raw/master/images/README_1.png" alt="1">
</center>

<A name="toc2-19" title="Contents" />
## Contents


**<a href="#toc2-24">Overview</a>**
*  <a href="#toc3-29">Scope and Goals</a>
*  <a href="#toc3-38">Getting started</a>
*  <a href="#toc3-53">Configuration</a>
&emsp;<a href="#toc4-56">Topics</a>
&emsp;<a href="#toc4-122">Projects</a>
*  <a href="#toc3-178">Code Generator</a>
&emsp;<a href="#toc4-199">Generate an app stub</a>
&emsp;<a href="#toc4-219">Generate example app</a>

**<a href="#toc2-252">Style Guide</a>**
*  <a href="#toc3-257">This Document</a>

<A name="toc2-24" title="Overview" />
## Overview

sDDS is a DDS implementation for sensor networks.

<A name="toc3-29" title="Scope and Goals" />
### Scope and Goals

sDDS has these goals:

* API compatible to DDS implementation
* Run on embedded devices as low as 8-bit micro controllers
* ...

<A name="toc3-38" title="Getting started" />
### Getting started

To get started with your first app add it to the 'example/examples.xml':

```xml
<examples>
    ...
    <example name = "my_example" os = "myos">My sDDS example</example>
    ...
</examples>
```

Then run the 'example/generate_examples.sh' to create the stub for your app.

<A name="toc3-53" title="Configuration" />
### Configuration

<A name="toc4-56" title="Topics" />
#### Topics

Topics are grouped by profiles and are configured in a xml file. By convention
these files are placed into 'example/topics' to avoid topic and profile
duplication as well as copy\&paste madness. The following example is taken from
`sdds/example/topics/alphabet.xml`:

```xml
<profile name = "alphabet">

    <topic name = "alpha" domain = "1" id = "A">
        Alpha messung topic.
        <attribute name = "value" type = "DDS_char">Any char value</attribute>
        <attribute name = "variant" type = "variant"></attribute>
        <attribute name = "parent" type = "parent"></attribute>
        <attribute name = "child" type = "child"></attribute>
    </topic>

    <topic name = "beta" domain = "1" id = "B">
        Beta messung topic.
        <attribute name = "value" type = "DDS_char">Any char value</attribute>
    </topic>

    <topic name = "gamma" domain = "1" id = "C">
        Gamma messung topic.
        <attribute name = "value" type = "DDS_char">Any char value</attribute>
    </topic>

    <topic name = "delta" domain = "1" id = "D">
        Delta messung topic.
        <attribute name = "value" type = "DDS_char">Any char value</attribute>
    </topic>

    <!-- Enums have a mandatory name and a optional namespace. You can have as
         many literals as an uint64 can hold. -->
    <enum name = "variant" namespace = "greek">
        Letter variants can be upper or lower case.
        <literal name = "upper" />
        <literal name = "lower" />
    </enum>

    <enum name = "parent" namespace = "greek">
        Parent systems of the greek alphabet
        <literal name = "egyptian" />
        <literal name = "proto sinaitic" />
        <literal name = "phoenician" />
    </enum>

    <!-- Enums can be further customized by assigning a unsigned integer. The
         largest value determines the data type (uint8 - uint64). -->
    <enum name = "child" namespace = "greek">
        Child systems of the greek alphabet
        <literal name = "coptic" value = "200" />
        <literal name = "gothic" value = "350" />
        <literal name = "glagolitic" value = "862" />
        <literal name = "cyrillic" value = "940" />
        <literal name = "armenian" value = "405" />
        <literal name = "old italic" value = "1000" />
        <literal name = "latin" value = "100" />
        <literal name = "georgian" value = "430" />
    </enum>
</profile>

```

<A name="toc4-122" title="Projects" />
#### Projects

To configure the example project use the generated sdds.xml and change it to
your needs.

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
    <role topic = "alpha" type = "publisher"/>

    <!--
		 static topic subscriptions
		if a subcription should be static, it can be set here. This
        should only be set, if the role is publisher
		<subscription topic = "mytopic">
			<subscriber ip = "fe80::20"/>
			<subscriber ip = "fe80::30"/>
		</subscription>
	-->
	<subscription topic = "alpha">
		<subscriber ip = "fe80::12:13ff:fe14:1516"/>
	</subscription>
	
</project>
```

Once you're done configuring your example run 'generate.sh' to create the
Makefile after that you can you 'make' and 'make clean' to build your example.

NOTE: The first time you're running 'generate.sh' it will download and install
the code generator which will take some time!

<A name="toc3-178" title="Code Generator" />
### Code Generator

The sDDS Code Generator is responsible for generating DDS topic and DDS
participant dependent code. It uses [GSL/4.1](https://github.com/imatix/gsl) as
code construction tool.

The sDDS Code Generator has these primary goals:

* generate topic files
* generate topic implementations
* generate Makefiles
* generate skeletons for new apps

Makefile generation is currently supported for:

* Linux
* Contiki

The code generator is split into two distinct generation processes:

<A name="toc4-199" title="Generate an app stub" />
#### Generate an app stub

<center>
<img src="https://zenon.cs.hs-rm.de/sdds/sdds/raw/master/images/README_2.png" alt="2">
</center>

This is done by 'sdds_examples.gsl'.

<A name="toc4-219" title="Generate example app" />
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

<A name="toc2-252" title="Style Guide" />
## Style Guide

sDDS uses [uCLASS](https://zenon.cs.hs-rm.de/sdds/sdds/blob/master/style_guide.md) guild for code style.

<A name="toc3-257" title="This Document" />
### This Document

_This documentation was generated from sdds/README.txt using [Gitdown](https://github.com/zeromq/gitdown)_