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
        self.nodes = {}
        self.graph = [] # 12 columns, 13th column will be adjacency list

    def CountWord(self, filename):
    	with open(filename) as f:
        	return Counter(chain.from_iterable(map(str.split, f))) 

    def BuildNodes( self, filename, Print=False ):
        with open(filename, 'rb') as f:
            reader = csv.reader(f, delimiter='|', quoting=csv.QUOTE_NONE)
            count = -1
            for row in reader:
                if count>=0:
                    row.append([])
                    self.graph.append(row)
                    self.nodes[row[0]] = count
                count += 1

    def GetDir( self, filename, txt_dir, Print=False ):
        with open(filename, 'rb') as f:
            reader = csv.reader(f, delimiter='|', quoting=csv.QUOTE_NONE)
            first = True
            idx_begin = len(txt_dir)+1
            count = -1
            for row in reader:
                if count>=0:
                    #print row
                    if row[2] != '':
	                hosts = subprocess.check_output('grep -r -m1 "'+row[1] \
                                +' '+row[2]+' '+row[3]+'" '+txt_dir, shell=True)
                    else:
	                hosts = subprocess.check_output('grep -r -m1 "'+row[1] \
                                +' '+row[3]+'" '+txt_dir, shell=True)
                    print hosts
                    print hosts.count('\n')
                    if hosts.count('\n') > 1:
                        hosts = hosts.splitlines()
                        for host in hosts:
                            idx_end = host.find(':')
                            target = host[idx_begin:idx_end]
                            if target!=row[0]:
                                target_count = self.nodes[target]
                                self.graph[count][13].append(target_count)
                                print "edge added: "+target
                count += 1

    def Print( self, txtobjs ):
        print txtobjs.get('file',          '') + '|'\
            + txtobjs.get('number',        '') + '|'\
            + txtobjs.get('prefix',        '') + '|'\
            + txtobjs.get('street',        '') + '|'\
            + txtobjs.get('type',          '') + '|'\
            + txtobjs.get('suffix',        '') + '|'\
            + txtobjs.get('sec_unit_type', '') + '|'\
            + txtobjs.get('sec_unit_num',  '') + '|'\
            + txtobjs.get('city',          '') + '|'\
            + txtobjs.get('state',         '') + '|'\
            + txtobjs.get('zip',           '') + '|'\
            + txtobjs.get('zip_ext',       '')
def main():

    # Test case
    #GetDir( "../samples/dox" )
    #GetDir( "../grouped/dox" )

    if len(sys.argv)<2:
        print "Usage: python build_txt.py ../samples/address.csv ../samples/dox10"

    graph = Builder()
    graph.GetDir( sys.argv[1], sys.argv[2] )

if __name__ == "__main__":
    main()
