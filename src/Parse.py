#!/usr/bin/env python
# coding: utf-8
#
import nltk


file = open("Kamonra.txt","r")
a = file.read()
text = nltk.word_tokenize(a)
pos_tags = nltk.pos_tag(text)
pos_tags = pos_tags('ascii','ignore')
nltk.ne_chunk(pos_tags, binary=True)




