#!/usr/bin/env python
# coding: utf-8
#
import os
import sys
import copy
import csv

class Builder:

    def __init__( self ):
        self.nodes = {}
        self.graph = []

    def CountWord(self, filename):
    	with open(filename) as f:
        	return Counter(chain.from_iterable(map(str.split, f))) 

    def GetDir( self, filename, Print=False ):
        with open(filename, 'rb') as f:
            reader = csv.reader(f, delimiter='|', quoting=csv.QUOTE_NONE)
            for row in reader:
                print row


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
        print "Usage: python build_txt.py ../samples/address.csv"

    graph = Builder()
    graph.GetDir( sys.argv[1] )

if __name__ == "__main__":
    main()
