#!/bin/sh
xtensa-lx106-elf-size build/axtls.a build/core.a build/freertos.a build/lwip.a build/program.a build/objs-esp/*.o > Messung1_ESP.csv
