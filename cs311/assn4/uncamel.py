#Python script to un-camelcase input

import re
import sys


def convert(name):
    s1 = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', name)
    return re.sub('([a-z0-9])([A-Z])', r'\1_\2', s1).lower()

out_file = open('temp.txt', 'w')

with open(sys.argv[1], 'r') as f:
	out_file.write(convert(f.read()))

