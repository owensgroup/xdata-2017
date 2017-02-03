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
        self.nvert = 0
        self.nedge = 0

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
            self.nvert = count
            #print self.nodes
            
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
                        try:
	                    hosts = subprocess.check_output('grep -r -m1 "'+row[1] \
                                    +' '+row[2]+' '+row[3]+'" '+txt_dir, shell=True)
                        except subprocess.CalledProcessError:
                            hosts = ''
                    else:
                        try:
	                    hosts = subprocess.check_output('grep -r -m1 "'+row[1] \
                                    +' '+row[3]+'" '+txt_dir, shell=True)
                        except subprocess.CalledProcessError:
                            hosts = ''
                    #print hosts
                    #print hosts.count('\n')
                    if hosts.count('\n') > 1:
                        hosts = hosts.splitlines()
                        for host in hosts:
                            idx_end = host.find(':')
                            target = host[idx_begin:idx_end]
                            if target!=row[0]:
                                target_count = self.nodes.get(target,None)
                                if target_count == None:
                                    target_count = len(self.nodes)
                                    self.nodes[target] = target_count
                                    new_row = [target]
                                    #new_row.append(target)
                                    #print target_count, new_row
                                    self.graph.append( new_row )

                                self.graph[count][12].append(target_count)
                                #print "edge added: "+target
                                self.nedge += 1
                                print count+1, target_count+1
                count += 1

    def Print( self, filename ):
        old_nvert = self.nvert
        self.nvert = len( self.nodes )
        print self.nvert, self.nvert, self.nedge

    # TODO: 1) fix append to different file
    #       2) add SSN support
    #       3) add name support

        with open(filename[:-4]+'_grep.csv', "w") as myfile:
            myfile.write(str(old_nvert)+' '+str(self.nvert)+'\n')
            count=0
            for row in self.graph:
                #print row[0]
                myfile.write(str(count)+' '+row[0]+'\n')
                count+=1
        #os.system('cat '+filename+' temp.csv > '+filename[:-4]+'_grep.csv')
        #os.system('rm temp.csv')

def main():

    # Test case
    #GetDir( "../samples/dox" )
    #GetDir( "../grouped/dox" )

    if len(sys.argv)<2:
        print "Usage: python build_txt.py ../samples/address.csv ../samples/dox10"

    graph = Builder()
    graph.BuildNodes( sys.argv[1], sys.argv[2] )
    graph.GetDir( sys.argv[1], sys.argv[2] )
    graph.Print( sys.argv[1] ) 

if __name__ == "__main__":
    main()
