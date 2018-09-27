'''
Created on Nov 22, 2013

@author: mlenart
'''

import os
import sys
import logging
from optparse import OptionParser

ARRAY_SIZE = 0x4000

def _parseOptions():
    """
    Parses commandline args
    """
    parser = OptionParser()
#     parser.add_option('-i', '--input-file',
#                         dest='inputFile',
#                         metavar='FILE',
#                         help='path to input file (CaseFolding.txt)')
    parser.add_option('-o', '--output-file',
                        dest='outputFile',
                        metavar='FILE',
                        help='path to output C++ source file')
#     parser.add_option('--header-filename', 
#                         dest='headerFilename',
#                         help='name of the C++ header file')
    
    
    opts, args = parser.parse_args()
    
    if None in [opts.outputFile, opts.constName, opts.headerFilename]:
        logging.error('Missing some options')
        parser.print_help()
        exit(1)
    return opts

def _parseCaseFoldingTxtFile(f):
    table = [code for code in range(ARRAY_SIZE)]
    extendedTable = {}
    for line in f:
        line = line.strip()
        if line and not line.startswith('#'):
            split = line.split('; ')
            code = int(split[0], 16)
            if split[1] in 'CS':
                targetCode = int(split[2], 16)
                if code < ARRAY_SIZE:
                    table[code] = targetCode
                else:
                    extendedTable[code] = targetCode
    return table, extendedTable

def _parseUnicodeDataTxtFile(f):
    toLowerTable = [code for code in range(ARRAY_SIZE)]
    extToLowerTable = {}
    toTitleTable = [code for code in range(ARRAY_SIZE)]
    extToTitleTable = {}
    for line in f:
        line = line.strip()
        if line and not line.startswith('#'):
            split = line.split(';')
            code = int(split[0], 16)
            lowercaseCode = int(split[13], 16) if split[13] else code
            titlecaseCode = int(split[14], 16) if split[14] else code
            if lowercaseCode != code:
                if code < ARRAY_SIZE:
                    toLowerTable[code] = lowercaseCode
                else:
                    extToLowerTable[code] = lowercaseCode
            if titlecaseCode != code:
                if code < ARRAY_SIZE:
                    toTitleTable[code] = titlecaseCode
                else:
                    extToTitleTable[code] = titlecaseCode
    return toLowerTable, extToLowerTable, toTitleTable, extToTitleTable
    

def _serializeTable(table):
    res = []
    res.append('{')
    for targetCode in table:
        res.append(str(targetCode))
        res.append(',')
    res.append('}')
    return ''.join(res)

def _serializeExtendedTable(table):
    res = []
    res.append('{')
    for code, targetCode in table.iteritems():
        res.append('{')
        res.append(str(code))
        res.append(',')
        res.append(str(targetCode))
        res.append('},')
    res.append('}')
    return ''.join(res)

def _serialize(toLowerTable, extToLowerTable, toTitleTable, extToTitleTable):
    return '''
#include "case_folding.hpp"

const unsigned int TO_LOWERCASE_TABLE_SIZE = {toLowerTableSize};
const unsigned int EXT_TO_LOWERCASE_TABLE_SIZE = {extToLowerTableSize};
const uint32_t TO_LOWERCASE_TABLE[] = {toLowerTable};
const uint32_t EXT_TO_LOWERCASE_TABLE[][2] = {extToLowerTable};

const unsigned int TO_TITLECASE_TABLE_SIZE = {toTitleTableSize};
const unsigned int EXT_TO_TITLECASE_TABLE_SIZE = {extToTitleTableSize};
const uint32_t TO_TITLECASE_TABLE[] = {toTitleTable};
const uint32_t EXT_TO_TITLECASE_TABLE[][2] = {extToTitleTable};
'''.format(
           toLowerTableSize=len(toLowerTable), 
           toLowerTable=_serializeTable(toLowerTable),
           extToLowerTableSize=len(extToLowerTable),
           extToLowerTable=_serializeExtendedTable(extToLowerTable),
           toTitleTableSize=len(toTitleTable),
           toTitleTable=_serializeTable(toTitleTable),
           extToTitleTableSize=len(extToTitleTable),
           extToTitleTable=_serializeExtendedTable(extToTitleTable))

if __name__ == '__main__':
    outfile = sys.argv[1]
    with open(os.path.join(os.path.dirname(__file__), 'UnicodeData.txt'), 'r') as f:
        toLowerTable, extToLowerTable, toTitleTable, extToTitleTable = _parseUnicodeDataTxtFile(f)
        with open(sys.argv[1], 'w') as f1:
            f1.write(_serialize(toLowerTable, extToLowerTable, toTitleTable, extToTitleTable))
