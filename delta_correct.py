#!/usr/bin/env python

import sys

last_correct = 0

for line in sys.stdin.readlines():
    if line[0] == '#':
        continue
    vals = line.split()
    if len(vals) < 2:
        continue
    if vals[1] == '1':
        print(float(vals[0]) - last_correct)
        last_correct = float(vals[0])

