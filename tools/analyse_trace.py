#!/usr/bin/python
import csv
import numpy as np
import sys
import getopt

dict_gray_bin= {'00000':'00000',
                '00001':'00001',
                '00003':'00002',
                '00002':'00003',
                '00006':'00004',
                '00007':'00005',
                '00005':'00006',
                '00004':'00007',
                '0000C':'00008',
                '0000D':'00009',
                '0000F':'0000A',
                '0000E':'0000B',
                '0000A':'0000C',
                '0000B':'0000D',
                '00009':'0000E',
                '00008':'0000F'}

def print_usage():
    print "analyse_tarce.py -t <tracefile> -m <tracemode>"
    print "\t OPTIONS:"
    print "\t\t -t <tracefile> select the tracefile which should be analysed."
    print "\t\t -m <tracemode> tell the script which type of trace you record, possible values are:"
    print "\t\t\t RECV_N: normal recv."
    print "\t\t\t RECV_I: isolated recv."
    print "\t\t\t SEND_N: normal write."
    print "\t\t\t SEND_I: isolated write."

def main(argv):
    NUM_EVENTS = 16
    name = 'Bus.csv'
    mode = "RECV_N"
    modes = ["RECV_N","RECV_I","SEND_N","SEND_I"]
    try:
        opts, args = getopt.getopt(argv,"ht:m:")
    except getopt.GetoptError:
        print_usage()
        sys.exit(2)
    for opt, arg in opts:
        if opt == "-h":
            print_usage()
            sys.exit()
        elif opt == "-t":
            name = arg
        elif opt == "-m":
            if  not arg in modes:
                print_usage()
                sys.exit(2)
            if arg == "RECV_N" or arg == "SEND_N":
                NUM_EVENTS = 8
            mode = arg
        else:
            print_usage()
            sys.exit(2)

    if len(sys.argv) == 2:
        name = str(sys.argv[1])

    time_dict = {}

    start = False
    last_row = None

    with open(name, 'rb') as csvfile:
        bistScopeReader = csv.DictReader(csvfile, delimiter='\t', quoting=csv.QUOTE_NONE)
        for row in bistScopeReader:
            if not start:
                if dict_gray_bin[row['Bus']] == '00000':
                    start = True
            else:
                time = float(row['Time'])
                # don't calc errors
                if time <= 10.0:
                    if last_row['Bus'] in time_dict:
                    	print last_row['Bus']
                    	print time_dict[last_row['Bus']][-1]
                        #del time_dict[last_row['Bus']][-1]
                    continue
                elif mode ==  "RECV_I":
                    if (int(dict_gray_bin[last_row['Bus']], base=16) + 1) % NUM_EVENTS == int(dict_gray_bin[row['Bus']], base=16) and\
                      dict_gray_bin[row['Bus']] != '00001' and\
                      dict_gray_bin[row['Bus']] != '00000' and\
                      dict_gray_bin[row['Bus']] != '0000F':
                        if dict_gray_bin[row['Bus']] in time_dict.keys():
                            time_dict[dict_gray_bin[row['Bus']]].append(float(row['Time'])/1000)
                        else:
                            time_dict[dict_gray_bin[row['Bus']]] = [float(row['Time'])/1000]
                elif mode == "RECV_N":
                    if (int(dict_gray_bin[last_row['Bus']], base=16) + 1) % NUM_EVENTS == int(dict_gray_bin[row['Bus']], base=16) and\
                      dict_gray_bin[row['Bus']] != '00001' and\
                      dict_gray_bin[row['Bus']] != '0000F':
                        if dict_gray_bin[row['Bus']] in time_dict.keys():
                            time_dict[dict_gray_bin[row['Bus']]].append(float(row['Time'])/1000)
                        else:
                            time_dict[dict_gray_bin[row['Bus']]] = [float(row['Time'])/1000]
                elif mode == "SEND_N":
                    if (int(dict_gray_bin[last_row['Bus']], base=16) + 1) % NUM_EVENTS == int(dict_gray_bin[row['Bus']], base=16) and\
                      dict_gray_bin[row['Bus']] != '00001' and\
                      dict_gray_bin[row['Bus']] != '00000' and\
                      dict_gray_bin[row['Bus']] != '00007' :
                        if dict_gray_bin[row['Bus']] in time_dict.keys():
                            time_dict[dict_gray_bin[row['Bus']]].append(float(row['Time'])/1000)
                        else:
                            time_dict[dict_gray_bin[row['Bus']]] = [float(row['Time'])/1000]
                elif mode == "SEND_I":
                    if (int(dict_gray_bin[last_row['Bus']], base=16) + 1) % NUM_EVENTS == int(dict_gray_bin[row['Bus']], base=16) and \
                        dict_gray_bin[row['Bus']] != '00001' and\
                        dict_gray_bin[row['Bus']] != '00000' and\
                        dict_gray_bin[row['Bus']] != '0000A' and\
                        dict_gray_bin[row['Bus']] != '0000B' and\
                        dict_gray_bin[row['Bus']] != '0000C' and\
                        dict_gray_bin[row['Bus']] != '0000D' and\
                        dict_gray_bin[row['Bus']] != '0000E' and\
                        dict_gray_bin[row['Bus']] != '0000F':
                        if dict_gray_bin[row['Bus']] in time_dict.keys():
                            time_dict[dict_gray_bin[row['Bus']]].append(float(row['Time'])/1000)
                        else:
                            time_dict[dict_gray_bin[row['Bus']]] = [float(row['Time'])/1000]
            last_row = row;


    for k in sorted(time_dict.keys()):
        print 'Event: ' + str((int(k, base=16) - 1) % NUM_EVENTS) + ' to Event: ' + str((int(k, base=16)))
        print 'Samples:' + str(len(time_dict[k]))
        print 'AVG: ' + str(reduce(lambda x, y: x + y, time_dict[k]) / len(time_dict[k])) + ' us'
        print "STD: " + str(np.std(time_dict[k])) + ' us'
        print "MIN: " + str(np.min(time_dict[k])) + ' us'
        print "MAX: " + str(np.max(time_dict[k])) + ' us'
        print ''

if __name__ == "__main__":
   main(sys.argv[1:])
