'''
Created on 20 lut 2014

@author: mlenart
'''
import logging
from morfeuszbuilder.utils.serializationUtils import htons, htonl
from morfeuszbuilder.utils import serializationUtils
from morfeuszbuilder.utils import exceptions
import shiftOrthMagic

class RulesManager(object):
    
    def __init__(self, segtypes, separatorsList):
        self.options2DFA = {}
        self.segtypes = segtypes
        self.separatorsList = separatorsList
        self.defaultOptions = None
        self.shiftOrthMagic = shiftOrthMagic.ShiftOrthMagic()
    
    def _options2Key(self, optionsMap):
        return frozenset(optionsMap.items())
    
    def _key2Options(self, optionsKey):
        return dict(optionsKey)
    
    def getDFA(self, optionsMap):
        return self.options2DFA[self._options2Key(optionsMap)]
    
    def setDefaultOptions(self, key2Def):
        self.defaultOptions = key2Def
    
    def addDFA(self, optionsMap, dfa):
        self.options2DFA[self._options2Key(optionsMap)] = dfa
    
    def lexeme2SegmentTypeNum(self, lemma, tagnum, namenum, labelsnum):
        res = self.segtypes.lexeme2Segnum(lemma, tagnum, namenum, labelsnum)
        if res is None:
            raise ValueError()
        else:
            return res
    
    def serialize(self):
        res = bytearray()
        res.extend(self._serializeSeparatorsList())
        dfasNum = len(self.options2DFA)
        exceptions.validate(
                            dfasNum > 0 and dfasNum < 256,
                            u'Too many segmentation rules variants')
        res.append(dfasNum)
        for key, dfa in self.options2DFA.iteritems():
            optionsMap = self._key2Options(key)
            res.extend(self._serializeOptionsMap(optionsMap))
            res.extend(self._serializeDFA(dfa))
        res.extend(self._serializeOptionsMap(self.defaultOptions))
        logging.info('segmentation rules size: %s bytes', len(res))
#         logging.info([int(x) for x in res])
        return res
    
    def _serializeSeparatorsList(self):
        res = bytearray()
        res.extend(serializationUtils.htons(len(self.separatorsList)))
        for cp in sorted(self.separatorsList):
            res.extend(serializationUtils.htonl(cp))
        return res
    
    def _serializeOptionsMap(self, optionsMap):
        assert len(optionsMap) < 256
        res = bytearray()
        res.append(2)
        res.extend(self._serializeString('aggl'))
        res.extend(self._serializeString(optionsMap['aggl']))
        res.extend(self._serializeString('praet'))
        res.extend(self._serializeString(optionsMap['praet']))
        return res
    
    def _serializeDFA(self, dfa):
        res = bytearray()
        dfaBytearray = dfa.serialize()
        res.extend(htonl(len(dfaBytearray)))
        res.extend(dfaBytearray)
#         print [x for x in dfaBytearray]
        return res
    
    def _serializeString(self, string):
        res = bytearray()
#         res.append(len(string))
        res.extend(string.encode('utf8'))
        res.append(0)
        return res
