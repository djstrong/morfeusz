'''
Created on Nov 7, 2013

@author: mlenart
'''
import sys

if __name__ == '__main__':
    version = sys.argv[1]
    res = set()
    print '#morfeusz-tagset', version
    for line in sys.stdin:
        if line.strip():
            tag = line.split('\t')[2]
            res.add(tag)
    for idx, tag in enumerate(sorted(res)):
        print str(idx) + '\t' + tag
