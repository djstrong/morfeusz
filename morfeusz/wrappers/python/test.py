# -*- coding: utf-8 -*-
import morfeusz2

import unittest
import tempfile
import os
import sys

class TestSequenceFunctions(unittest.TestCase):

    def setUp(self):
        self.morfeusz = morfeusz2.Morfeusz.createInstance()

    def testAnalyzeAsList(self):
        res = self.morfeusz.analyse("Aaaa żżżż");
        self.assertEquals(2, len(res));
        self.assertEquals(u"Aaaa", res[0].orth);
        self.assertEquals(u"żżżż", res[1].orth);
        try:
            res[2];
            fail();
        except IndexError:
            pass
    
    def testAnalyzeAsIterator(self):
        res = list(self.morfeusz.analyse_iter("Aaaa żżżż"));
        self.assertEquals(2, len(res));
        self.assertEquals(u"Aaaa", res[0].orth);
        self.assertEquals(u"żżżż", res[1].orth);
        try:
            res[2];
            fail();
        except IndexError:
            pass
            
    def testInvalidAgglOption(self):
        try:
            self.morfeusz.setAggl('XXXXYYYYZZZZ')
            self.fail()
        except RuntimeError:
            pass
    
    def testInvalidPraetOption(self):
        try:
            self.morfeusz.setPraet('XXXXYYYYZZZZ')
            self.fail()
        except RuntimeError:
            pass
    
    def testInvalidGenerate(self):
        try:
            self.morfeusz.generate("AAAA BBBB")
            self.fail()
        except RuntimeError:
            pass
    
    def testValidCaseHandling(self):
        self.morfeusz.setCaseHandling(morfeusz2.IGNORE_CASE)

    def testInvalidCaseHandling(self):
        try:
            self.morfeusz.setCaseHandling(0)
            self.fail()
        except ValueError:
            pass
    
    def testInvalidTokenNumbering(self):
        try:
            self.morfeusz.setTokenNumbering(0)
            self.fail()
        except ValueError:
            pass
    
    def testInvalidWhitespaceHandling(self):
        try:
            self.morfeusz.setWhitespaceHandling(0)
            self.fail()
        except ValueError:
            pass
        
    def testNonExistingDictionaryFile(self):
        try:
            self.morfeusz.setDictionary("1P4sEBuWv")
            self.fail()
        except IOError:
            pass
    
    def testInvalidDictionaryFile(self):
        dirpath = tempfile.mkdtemp()
        dictName = '6J1vMiqY'
        path = os.path.join(dirpath, dictName + '-a.dict')
        with open(path, "a+") as f:
            f.write('ee2rmtsq')
        try:
            self.morfeusz.setDictionary(dictName)
            self.fail()
        except IOError:
            pass

if __name__ == '__main__':
    unittest.main()

