#!/usr/bin/python
import csv
import numpy as np

STATE_COUNT = 4


list_data =  []

with open('BitScope_trace.csv', 'rb') as csvfile:
     bistScopeReader = csv.DictReader(csvfile, delimiter=',', quoting=csv.QUOTE_NONE)
     
     for row in bistScopeReader:
        if row['channel'] == '4':
          data = {}
          samples  = {}
          samples['4'] = []
          samples['4'].append(row['data'])
          samples['4'].extend(row[None])
          samples['5'] = []
          samples['6'] = []
          samples['7'] = []
          factor = int(row['factor'])
          if 'E' in row['rate']:
            split_rate = row['rate'].split('E',1)
            data['rate'] = float(1) / (float(split_rate[0]) * 10**float(split_rate[1]) * factor) 
          else:
            data['rate'] = float(1) / (float(row['rate']) * factor)
        elif row['channel'] == '5' or row['channel'] == '6' or row['channel'] == '7':
          samples[row['channel']].append(row['data'])
          samples[row['channel']].extend(row[None])
          if row['channel'] == '7':
            data['data'] = samples
            list_data.append(data)
        else:
          continue

state_path = range(1,STATE_COUNT)
state = 0
count = 0
state_times = {}
print state_path
#iterate over states and init list for the state times
for x in state_path[:3]:
  state_times[str(x)] = []

#print list_data
data = {}
for sample in list_data:
  data = sample['data']
  data_channel0 = data['4']
  data_channel1 = data['5']
  data_channel2 = data['6']
  data_channel3 = data['7']

  for i in xrange(len(data_channel0)):
    bit_repr = 0
    if data_channel0[i] == '5':
      bit_repr |= (1 << 0)
    if data_channel1[i] == '5':
      bit_repr |= (1 << 1)
    if data_channel2[i] == '5':
      bit_repr |= (1 << 2)
    if data_channel3[i] == '5':
      bit_repr |= (1 << 3)
    #print "bit_repr: " + str(bit_repr) + " state_path[state]: " + str(state_path[state]) + " state: " + str(state)
    if state_path[state] == bit_repr:
      # reconize state
      state = (state + 1) % len(state_path) # next state
      if bit_repr == 1:
        #stop trace
        count = 0
      if bit_repr == 2 or bit_repr == 3:
        index = bit_repr - 1 # time for the last event
        state_times[str(index)].append(count * sample['rate'])
        #print count
        count = 0

    count += 1

for x in state_path[:2]:
  print "Event " + str(x)
  print 'Samples:' + str(len(state_times[str(x)]))
  if len(state_times[str(x)]) != 0:
    print "AVG: " + str(reduce(lambda x, y: x + y, state_times[str(x)]) / len(state_times[str(x)]))
  
    
