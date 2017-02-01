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
        if len(line)<200:
            line = line.split(':')
            for subline in line:
                addr = sa.parse(subline)
                if Print is True:
                    print addr
                # 1. Longest street in US is 34 chars
                # 2. If have 'number', 'street', 'type'
                #    then try to look at next row for 'city' and 'state'
                # 3. If only have 'city'
                #    then try to look at next row for 'state'
                if (addr is not None and len(addr)>=5) and \
                    ('street' in addr.keys() and len( addr['street'] )<=34): 
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
        print "       python read_txt.py -v ../samples/dox"

    if sys.argv[1]=='-v':
        GetDir( sys.argv[2], True )
    else:
        GetDir( sys.argv[1] )

if __name__ == "__main__":
    main()
