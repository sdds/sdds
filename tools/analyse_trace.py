#!/usr/bin/python
import csv
import numpy as np

data = {}
list_data =  []

with open('BitScope.csv', 'rb') as csvfile:
     bistScopeReader = csv.DictReader(csvfile, delimiter=',', quoting=csv.QUOTE_NONE)
     #print bistScopeReader
     samples  = {}
     for row in bistScopeReader:
      if row['channel'] == '4' or row['channel'] == '5' or row['channel'] == '6' or row['channel'] == '7':
        samples[row['channel']] = []
       	for header, value in row.items():
          if header == 'data':
            samples[row['channel']].append(value)
          elif header ==  None:
            samples[row['channel']].extend(value)
            data['data'] = samples
          elif header == 'rate':
            split_rate = row[header].split('E',1)
            data[header] = float(1) / (float(split_rate[0]) * 10**float(split_rate[1]))
            #print data[header]
          elif header != 'channel' and header != 'trigger' and header != 'delay' and header != 'factor' and header != 'stamp' and header != 'type' and header != 'index':
            data[header] =  value
          list_data.append(data)

print list_data