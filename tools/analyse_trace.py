#!/usr/bin/python
import csv
import numpy as np

STATE_COUNT = 4

data = {}
list_data =  []

with open('BitScope_trace.csv', 'rb') as csvfile:
     bistScopeReader = csv.DictReader(csvfile, delimiter=',', quoting=csv.QUOTE_NONE)
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

state_path = range(1,STATE_COUNT)
state = 0
count = 0
state_times = {}
print state_path
for x in state_path[:3]:
  state_times[str(x)] = []

for sample in list_data:
  data = sample['data']
  data_channel0 = data['4']
  data_channel1 = data['5']
  data_channel2 = data['6']
  data_channel3 = data['7']

  '''print "channel0"
  print data_channel0
  print "channel0"
  print data_channel
  print "channel2"
  print data_channel2
  print "channel3"
  print data_channel3'''

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
    print bit_repr
    if state_path[state] == bit_repr:
      # reconize state
      state = (state + 1) % len(state_path) # next state
      if bit_repr == 0 or bit_repr == 1:
        #stop trace
        count = 0
      if bit_repr == 2 or bit_repr == 3:
        index = bit_repr - 1 # time for the last event
        state_times[str(index)].append(count * sample['rate'])
        print count
        count = 0


    count += 1

for x in state_path[:3]:
  print "Event " + str(x)
  print 'Samples:' + str(len(state_times[str(x)]))
  #print "AVG: " + str(reduce(lambda x, y: x + y, state_times[str(x)]) / len(state_times[str(x)]))
  
    