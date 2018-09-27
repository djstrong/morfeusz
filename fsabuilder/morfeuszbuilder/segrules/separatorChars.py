'''
Created on 3 kwi 2014

@author: mlenart
'''

from morfeuszbuilder.utils import exceptions

def parseSeparatorChars(segtypesConfigFile):
    res = []
    for lineNum, line in segtypesConfigFile.enumerateLinesInSection('separator chars', ignoreComments=True):
        try:
            cp = int(line)
            res.append(cp)
        except ValueError as ex:
            raise exceptions.ConfigFileException(
                                                 segtypesConfigFile.filename, 
                                                 lineNum, 
                                                 str(ex))
    return res
    