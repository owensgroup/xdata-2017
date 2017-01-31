#!/usr/bin/env python
# coding: utf-8
#
import os
import sys
import streetaddress as sa

def ReadTxt( txtfile, Print=False ):
    #i = 0
    f = open(txtfile, 'r')
    for line in iter(f):
        if Print is True:
            print line
        line = line.split(':')
        for subline in line:
            #print subline
            addr = sa.parse(subline)
            if addr is not None and len(addr)>=5:
                print addr
                return addr
        #break
        #if i > 2:
        #    break
    #return txtobjs

def GetDir( txt_dir, Print=False ):
    txt_files = [pos_txt for pos_txt in os.listdir(txt_dir)]
    #print len(txt_files), txt_files
    for txt_file in txt_files:
	txt_path = txt_dir+'/'+txt_file
        #print txt_path
	txtobjs = ReadTxt( txt_path, Print )
    return

def main():

    # Test case
    #GetDir( "../samples/dox" )
    #GetDir( "../grouped/dox" )

    if len(sys.argv)<2:
        print "Usage: python read_txt.py ../samples/dox"

    GetDir( sys.argv[1] )

if __name__ == "__main__":
    main()
