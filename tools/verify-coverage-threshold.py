#!/usr/bin/python2

import sys
import xml.etree.ElementTree as ET

tree = ET.parse(sys.argv[1])
root = tree.getroot()
threshold = float(sys.argv[2])

line_coverage = float(tree.getroot().attrib['line-rate']) * 100
branch_coverage = float(tree.getroot().attrib['branch-rate']) * 100

print("-- Line coverage is %.2f%%" % round(line_coverage ,2))
print("-- Branch coverage is %.2f%%" % round(line_coverage ,2))

if line_coverage >= threshold:
    print("-- Coverage above threshold!")
    sys.exit(0)

print("-- Coverage requirements of %.2f%% not met!" % threshold)
sys.exit(1)
