#!/usr/bin/env python

resolution = 10
resistor = 5.4
ref_voltage = 1.79
stb_resistor = 47.0

directions_resistance = [
	[   0.0,     33.0 ],
	[  22.5, (33.0 * 8.2) / (33.0 + 8.2) ],
	[  45.0,     8.2 ],
	[  67.5, (8.2 * 1.0) / (8.2 + 1.0) ],
	[  90.0,     1.0 ],
	[ 112.5, (1.0 * 2.2) / (1.0 + 2.2) ],
	[ 135.0,     2.2 ],
	[ 157.5, (2.2 * 3.9) / (2.2 + 3.9) ],
	[ 180.0,     3.9 ],
	[ 202.5, (3.9 * 16) / (3.9 + 16.0) ],
	[ 225.0,     16.0 ],
	[ 247.5, (16.0 * 120.0) / (16.0 + 120.0) ],
	[ 270.0,     120.0 ],
	[ 292.5, (120.0 * 64.9) / (120.0 + 64.9) ],
	[ 315.0,     64.9 ],
	[ 337.5, (64.9 * 33.0) / (64.9 + 33) ]
]

directions_converted = []

print "%10s %10s %10s %10s" % ("degrees", "resistance", "digital", "voltage")

lowest_voltage = 0
highest_voltage = 0

for i in directions_resistance:
	in_resistor = stb_resistor * i[1] / (stb_resistor + i[1])
	#voltage = ref_voltage - (ref_voltage * resistor / (in_resistor + resistor))
	#converted = (2 ** resolution) * resistor / (in_resistor + resistor)
	voltage = in_resistor * ref_voltage / (in_resistor + resistor)
	converted = (2 ** resolution) * in_resistor / (in_resistor + resistor)
	directions_converted.append([i[0], i[1], converted, voltage])
	if voltage < lowest_voltage:
		lowest_voltage = voltage
	if voltage > highest_voltage:
		highest_voltage = voltage
	print "%10.4f %10.4f %10.4f %10.4f" % (i[0], i[1], converted, voltage)

print "range: %f" % (highest_voltage - lowest_voltage)

import sys

if len(sys.argv) <= 1:
	sys.exit(0)

for i in directions_converted:
	print i

arg = float(sys.argv[1])

print arg

nearest_direction = min(directions_converted, key = lambda value : abs(arg - value[1]))[0]
print nearest_direction
