#!/usr/bin/python 
import csv
import numpy as np


list_data =  []

with open('BitScope.csv', 'rb') as csvfile:
     bistScopeReader = csv.DictReader(csvfile, delimiter=',', quoting=csv.QUOTE_NONE)
     
     for row in bistScopeReader:
      data = {}
      if row['channel'] == '11':
        samples = []
        samples.append(row['data'])
        samples.extend(row[None])
        split_rate = row['rate'].split('E',1)
        data['rate'] = float(1) / (float(split_rate[0]) * 10**float(split_rate[1]))
        data['data'] = samples
        list_data.append(data)
      else:
        continue

last_x = '0'
count = 1
start_count = False
times= []
for sample in list_data:
	for x in sample['data']:
		if last_x == '0' and  x == '5':
			start_count = True
			count = 1
		if x == '5'and start_count:
			count += 1
		if last_x == '5' and  x == '0':
			time = int(count) * sample['rate']
			times.append(time)
			start_count = False
		last_x = x


#print times

print 'Samples:' + str(len(times))
print "AVG: " + str(reduce(lambda x, y: x + y, times) / len(times))
print "STD: " + str(np.std(times))
print "MIN: " + str(np.min(times))
print "MAX: " + str(np.max(times))





