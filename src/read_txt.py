#!/usr/bin/env python
# coding: utf-8
#
import os
import sys
import copy
import streetaddress as sa
from nltk.tag import StanfordNERTagger

class Parser:

    def __init__( self ):
        self.st = StanfordNERTagger('english.conll.4class.distsim.crf.ser.gz')

    def ReadFastTxt( self, txtfile, Print=False ):
        f = open(txtfile, 'r')
        for line in iter(f):
            if len(line)<200:
                line = line.split(':')
                for subline in line:
                    addr = sa.parse(subline)
                    if (addr is not None and len(addr)>=5) and \
                        ('street' in addr.keys() and len( addr['street'] )<=34) and \
                        ('number' in addr.keys()) and \
                        ('city' in addr.keys()) and \
                        ('state' in addr.keys()):
                        return addr

    def ReadSlowTxt( self, txtfile, Print=False ):
        state = 0
        count = 0
        total_attempts=3
        good_addr=''
        prev_one_line=''
        prev_two_line=''
        f = open(txtfile, 'r')
        for line in iter(f):
            #if Print is True:
            #    print line
            prev_two_line = prev_one_line
            prev_one_line = line
            if count<0:
                state = 0
            if len(line)<200:
                print str(state)+'|'+ line+'|'+ good_addr
                if state==0:
                    line = line.split(':')
                    for subline in line:
                        addr = sa.parse(subline)
                        #print addr
                        if (addr is not None) and \
                                ('number' in addr.keys()) and \
                                ('street' in addr.keys()) and \
                                len(addr['street'])<=34:
                            if ('city' not in addr.keys()) and \
                                    ('state' not in addr.keys()):
                                good_addr = subline.replace("\r\n","")
                                #print 'good: '+good_addr
                                #print 'go to 1'
                                state = 1
                                count = total_attempts
                                break
                            if 'state' not in addr.keys():
                                good_addr = subline.replace("\r\n","")
                                #print 'good: '+good_addr
                                #print 'go to 2'
                                state = 2
                            else:
                                return addr
                            break

                else:
                    line = line.split()
                    tags = self.st.tag(line)
                    subline = good_addr
                    #print tags
                    #sub = []
                    #sub.append(good_addr)
                    for tag in tags:
                        if tag[1]=='LOCATION':
                            subline += ' '+tag[0].replace("\r\n","").encode('ascii')
                            #sub.append(tag[0].replace("\r\n",""))
                    #subline=''.join(sub)
                    #print state, subline
                    if state==1:
                        addr = sa.parse(subline)
                        #print state, addr
                        if (addr is not None) and \
                                ('number' in addr.keys()) and \
                                ('street' in addr.keys()) and \
                                ('city' in addr.keys()) and \
                                ('state' in addr.keys()) and \
                                len(addr['street'])<=34:
                            return addr
                        else:
                            state = 2
                            good_addr = subline
                            count-=1
                            #print 'go to 2'
                            continue
                    if state==2: 
                        addr = sa.parse(subline)
                        #print state, addr
                        if (addr is not None) and \
                                ('number' in addr.keys()) and \
                                ('street' in addr.keys()) and \
                                ('city' in addr.keys()) and \
                                len(addr['street'])<=34:
                                return addr
                        else:
                            count-=1
                            continue

                # Basic state machine with 3 states
                #
                # 0. state = 0, good_addr is None
                #    If have 'number', 'street', 'type'
                #      set state = 1
                #      go to next line in iter(f)
                #    If have 'number', 'street', 'type', 'city', 'state'
                #      return good_addr
                #      go to next line in iter(f)
                #    If have 'number', 'street', 'type', 'city',
                #      set state = 2
                #      go to next line in iter(f)
                # 1. state = 1, good_addr has 'number', 'street', 'type'
                #    If have 'city', 'state'
                #      return good_addr
                #      set state = 0
                #      go to next line in iter(f)
                #    If only have 'city'
                #      then try to look at next row for 'state'
                #      set state = 2
                # 2. state = 2, good_addr has 'number', 'street', 'type', 'city'
                #    If have 'state'
                #      return good_addr
                #      set state = 0
                #    go to next line in iter(f)
                #
                # Note: Longest street in US is 34 chars

    def GetDir( self, txt_dir, Print=False ):
        txt_files = [pos_txt for pos_txt in os.listdir(txt_dir)]
        #print len(txt_files), txt_files
        for txt_file in txt_files:
	    txt_path = txt_dir+'/'+txt_file
            if Print is True:
                print txt_path
    	    txtobjs = self.ReadFastTxt( txt_path, Print )
            if txtobjs is None:
    	        txtobjs = self.ReadSlowTxt( txt_path, Print )
            print txtobjs
        return

def main():

    # Test case
    #GetDir( "../samples/dox" )
    #GetDir( "../grouped/dox" )

    if len(sys.argv)<2:
        print "Usage: python read_txt.py ../samples/dox"
        print "       python read_txt.py -v ../samples/dox"

    parse = Parser()
    if sys.argv[1]=='-v':
        parse.GetDir( sys.argv[2], True )
    else:
        parse.GetDir( sys.argv[1] )

if __name__ == "__main__":
    main()
