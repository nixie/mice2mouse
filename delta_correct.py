#!/usr/bin/env python

import sys
import os
import string

minval = 0
pole = []
x = open('stats/stats.txt', 'w')
filecounter = 0

for dirname, dirnames, filenames in os.walk('./logs'):
    for filename in filenames:
        filecounter += 1
	last_correct = 0
	count = 0
	f = open(os.path.join(dirname, filename))
	for line in f:
	    if line[0] == '#':
		continue
	    vals = line.split()
	    if len(vals) < 2:
		continue
	    if vals[1] == '1':
		if minval == 0:
		    pole.append(float(vals[0]) - last_correct)
                elif count < minval:
		    pole[count] += float(vals[0]) - last_correct
		last_correct = float(vals[0])
	        count += 1
	if minval == 0 or count < minval:
	    minval = count
for line in pole:
    s = str(line / filecounter)
    x.write(s)
    x.write('\n')

