# sDDS - Code Generator

## Overview

The sDDS Code Generator is responsible for generating DDS topic and DDS participant dependent code. It uses [GSL/4.1](https://github.com/imatix/gsl) as code construction tool.

### Scope and Goals

The sDDS Code Generator has these primary goals:

* generate topic files
* generate topic implementations
* generate Makefiles
* generate skeletons for new apps

Makefile generation is currently supported for:

* Linux
* Contiki

### Getting started

To get started with your first app add it to the 'example/examples.xml':

```xml
<examples>
    ...
    <example name = "my_example" os = "myos">My sDDS example</example>
    ...
</examples
```

Then run the 'example/generate_examples.sh' to create the stub for your app.

## Configuration

### Topics

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

### Example

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

## Code Generator

The code generator is split into two distinct generation processes:

### Generate an example stub

This is done by 'sdds_examples.gsl'.

### Generate example app

The entry point is 'sdds.gsl':

* sdds_topic.gsl - Generate the topics
* sdds_impl.gsl - Generate the implementation of the topics
* sdds_constants.gsl - Generate the default constants for sDDS
* sdds_make\_$(OS).gsl - Generates the Makefiles for the different operating systems
* sdds_skeleton - Generates a skeleton for new examples dependent on topics and roles
