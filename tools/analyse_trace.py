#!/usr/bin/python
import csv
import numpy as np

STATE_COUNT = 4
time_dict = {}

with open('Bus.csv', 'rb') as csvfile:
    bistScopeReader = csv.DictReader(csvfile, delimiter='\t', quoting=csv.QUOTE_NONE)

    for row in bistScopeReader:
        #print row
        if row['Bus'] == '00002' or row['Bus'] == '00003':
            if row['Bus'] in time_dict.keys():
                time_dict[row['Bus']].append(float(row['Time'])/1000)
            else:
                time_dict[row['Bus']] = [float(row['Time'])/1000]



for k in time_dict.keys():
    print 'Event: ' + str((int(k) - 1))
    print 'Samples:' + str(len(time_dict[k]))
    print 'AVG: ' + str(reduce(lambda x, y: x + y, time_dict[k]) / len(time_dict[k])) + ' us'
