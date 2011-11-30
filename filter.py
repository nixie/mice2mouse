#!/usr/bin/env python

import sys
from scipy.ndimage.filters import generic_filter
from scipy.stats import gmean,cmedian

nums = []

for line in sys.stdin.readlines():
    if line[0] == '#':
        continue
    if len(line) < 1:
        continue
    nums.append(float(line))

for v in generic_filter(nums, gmean, 5):
    print(v)
