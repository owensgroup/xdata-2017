#!/usr/bin/env python
# coding: utf-8
#
import os
import sys
import copy
import csv
import subprocess

class Builder:

    def __init__( self ):
        self.blah = 0

    def GetDir( self, fileinput, Print=False ):
        filein = open(fileinput, "r")
        count = 0
        last_key = 1
        my_list = []
        for line in filein:
            row = line.split()
            if count==0:
                length = row[2]
                print line[:-1]
            else:
                if count==1:
                    my_list.append(int(row[1]))
                    last_key = row[0]
                else:
                    if count==int(length):
                        if row[0]==last_key:
                            my_list.append(int(row[1]))
                            my_list.sort()
                            print '\n'.join([str(last_key)+' '+str(val) for val in my_list])
                            return
                        else:
                            my_list.sort()
                            print '\n'.join([str(last_key)+' '+str(val) for val in my_list])
                            print str(row[0])+' '+str(row[1])
                            return
                    else:
                        if row[0]==last_key:
                            my_list.append(int(row[1]))
                        else:
                            my_list.sort()
                            print '\n'.join([str(last_key)+' '+str(val) for val in my_list])
                            last_key = row[0]
                            my_list[:] = []
                            my_list.append(int(row[1]))
            count+=1

def main():

    # Test case
    #GetDir( "../samples/dox" )
    #GetDir( "../grouped/dox" )

    if len(sys.argv)<2:
        print "Usage: python sanitize_graphy.py graph2.mtx"

    graph = Builder()
    os.system( "cat "+sys.argv[1]+" | sed '1h;1d;$!H;$!d;G' > temp.mtx" )
    graph.GetDir( "temp.mtx" )
    print ".mtx conversion successful!"

if __name__ == "__main__":
    main()
