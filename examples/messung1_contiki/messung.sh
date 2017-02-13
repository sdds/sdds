#!/bin/bash
 avr-strip *.co objs-128rfa1/*.o obj_avr-atmega128rfa1/*.o
 avr-size *.co objs-128rfa1/*.o obj_avr-atmega128rfa1/*.o > Messung_contiki.csv
