#!/usr/bin/python
import csv
import numpy as np
import sys

NUM_EVENTS = 7

time_dict = {}

name = 'Bus.csv'

if len(sys.argv) == 2:
    name = str(sys.argv[1])

with open(name, 'rb') as csvfile:
    bistScopeReader = csv.DictReader(csvfile, delimiter='\t', quoting=csv.QUOTE_NONE)
    for row in bistScopeReader:
        if row['Bus'] == '00002' or row['Bus'] == '00003' or row['Bus'] == '00004' or row['Bus'] == '00005' or row['Bus'] == '00006':
            time = float(row['Time'])
            if time > 10.0:
                if row['Bus'] in time_dict.keys():
                    time_dict[row['Bus']].append(float(row['Time'])/1000)
                else:
                    time_dict[row['Bus']] = [float(row['Time'])/1000]



for k in sorted(time_dict.keys()):
    print 'Event: ' + str((int(k) - 1) % NUM_EVENTS) + ' to Event: ' + str((int(k)))
    print 'Samples:' + str(len(time_dict[k]))
    print 'AVG: ' + str(reduce(lambda x, y: x + y, time_dict[k]) / len(time_dict[k])) + ' us'
    print "STD: " + str(np.std(time_dict[k])) + ' us'
    print "MIN: " + str(np.min(time_dict[k])) + ' us'
    print "MAX: " + str(np.max(time_dict[k])) + ' us'
    print ''
