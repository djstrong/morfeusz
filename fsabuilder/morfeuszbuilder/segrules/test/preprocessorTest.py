'''
Created on 18 lut 2014

@author: mlenart
'''
import unittest
import codecs
import os

from morfeuszbuilder.segrules import preprocessor
from morfeuszbuilder.utils import configFile


class Test(unittest.TestCase):


    def testPreprocess(self):
        filename = os.path.join(os.path.dirname(__file__), 'segmenty.dat')
        parsedFile = configFile.ConfigFile(filename, ['options', 'combinations', 'tags', 'lexemes', 'segment types'])
        linesEnum = parsedFile.enumerateLinesInSection('combinations')
        for lineNum, line in preprocessor.preprocess(linesEnum, ['extra', 'superextra']):
            print (lineNum, line)


if __name__ == "__main__":
    #import sys;sys.argv = ['', 'Test.testPreprocess']
    unittest.main()