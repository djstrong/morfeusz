'''
Created on Oct 23, 2013

@author: mlenart
'''

from morfeuszbuilder.utils.serializationUtils import htons, serializeString
from morfeuszbuilder.utils import limits, exceptions

MAX_QUALIFIERS_COMBINATIONS = 1024

class Encoder(object):
    '''
    classdocs
    '''


    def __init__(self, lowercase, encoding='utf8'):
        '''
        Constructor
        '''
        self.lowercase = lowercase
        self.encoding = encoding
        #~ self.qualifiersMap = { frozenset(): 0}
    
    def encodeWord(self, word, lowercase=True):
        assert type(word) == unicode
        res = bytearray(word.lower() if self.lowercase and lowercase else word, self.encoding)
        return res
    
    def encodeData(self, data):
        raise NotImplementedError()
    
    def decodeData(self, rawData):
        return NotImplementedError()

    def decodeWord(self, rawWord):
        return unicode(str(rawWord).strip('\x00'), self.encoding)
    
    def word2SortKey(self, word):
        normalizedWord = word.lower() if self.lowercase else word
        return normalizedWord.encode(self.encoding)
    
    def _encodeTypeNum(self, typenum):
        exceptions.validate(
                            typenum <= limits.MAX_SEGMENT_TYPES,
                            u'Too many segment types. The limit is %d' % limits.MAX_SEGMENT_TYPES)
        return bytearray([typenum])
    
    def _hasUpperPrefix(self, casePattern):
        for i in range(len(casePattern) + 1):
            if all(casePattern[:i]) and not any(casePattern[i:]):
                return True
        return False
    
    def _getUpperPrefixLength(self, casePattern):
        assert self._hasUpperPrefix(casePattern)
        for i in range(len(casePattern)):
            if not casePattern[i]:
                return i
        return len(casePattern)
    
    def _encodeTagNum(self, tagnum):
        res = bytearray()
        exceptions.validate(tagnum <= limits.MAX_TAGS, u'Too many tags. The limit is %d' % limits.MAX_TAGS)
        res.append((tagnum & 0xFF00) >> 8)
        res.append(tagnum & 0x00FF)
        return res
    
    def _encodeNameNum(self, namenum):
        exceptions.validate(namenum <= limits.MAX_NAMES, u'Too many named entity types. The limit is %d' % limits.MAX_NAMES)
        return bytearray([namenum])
    
    def _groupInterpsByType(self, interpsList):
        res = {}
        for interp in interpsList:
            res.setdefault(interp.typenum, [])
            res[interp.typenum].append(interp)
        return res
    
    def _doEncodeData(self, interpsList):
        
        assert type(interpsList) == frozenset
        
        segnum2Interps = self._groupInterpsByType(interpsList)
        
        res = bytearray()
        
        for typenum, interpsList in segnum2Interps.iteritems():
            res.extend(self._encodeInterps4Type(typenum, interpsList))
        del interpsList
        
        res = htons(len(res)) + res
        
        return res

class MorphEncoder(Encoder):
    
    def __init__(self, encoding='utf8'):
        super(MorphEncoder, self).__init__(True, encoding)
    
    def encodeData(self, interpsList):
        return self._doEncodeData(interpsList)
    
    def _getMinOrthCasePatterns(self, interpsList):
        res = []
        for interp in interpsList:
            if not True in interp.orthCasePattern:
                return []
            else:
                res.append(list(interp.orthCasePattern))
        return res
    
    def _encodeCasePattern(self, casePattern):
        
        LEMMA_ONLY_LOWER = 0
        LEMMA_UPPER_PREFIX = 1
        LEMMA_MIXED_CASE = 2
        
        res = bytearray()
        if True not in casePattern:
            res.append(LEMMA_ONLY_LOWER)
            return res
        elif self._hasUpperPrefix(casePattern):
            res.append(LEMMA_UPPER_PREFIX)
            res.append(self._getUpperPrefixLength(casePattern))
            return res
        else:
            assert len(casePattern) < 256
            res.append(LEMMA_MIXED_CASE)
            res.append(len([c for c in casePattern if c]))
            for idx in range(len(casePattern)):
                if casePattern[idx]:
                    res.append(idx)
            return res
    
    def _casePatternsHaveOnlyLowercase(self, casePatterns):
        return not any(map(lambda cp: cp and True in cp, casePatterns))
    
    def _casePatternsAreOnlyTitles(self, casePatterns):
        return all(map(lambda cp: cp and cp[0] == True and not True in cp[1:], casePatterns))
    
    def _casePatternsAreEncodedInCompressByte(self, casePatterns):
        return self._casePatternsHaveOnlyLowercase(casePatterns) or self._casePatternsAreOnlyTitles(casePatterns)
    
    def _prefixCutsAreEncodedInCompressByte(self, prefixCuts):
        return len(prefixCuts) == 1 and list(prefixCuts)[0] < 15
    
    def _encodeCompressByte(self, orthCasePatterns, lemmaCasePatterns, prefixCuts):
        ORTH_ONLY_LOWER = 128
        ORTH_ONLY_TITLE = 64
        LEMMA_ONLY_LOWER = 32
        LEMMA_ONLY_TITLE = 16
        PREFIX_CUT_MASK = 15
        res = 0
        if self._casePatternsHaveOnlyLowercase(orthCasePatterns):
            res |= ORTH_ONLY_LOWER
        elif self._casePatternsAreOnlyTitles(orthCasePatterns):
            res |= ORTH_ONLY_TITLE
        if self._casePatternsHaveOnlyLowercase(lemmaCasePatterns):
            res |= LEMMA_ONLY_LOWER
        elif self._casePatternsAreOnlyTitles(lemmaCasePatterns):
            res |= LEMMA_ONLY_TITLE
        
        if self._prefixCutsAreEncodedInCompressByte(prefixCuts):
            res |= list(prefixCuts)[0]
        else:
            res |= PREFIX_CUT_MASK
        
        return res
    
    def _encodeInterps4Type(self, typenum, interpsList):
        res = bytearray()
        res.extend(self._encodeTypeNum(typenum))
        
        encodedInterpsList = bytearray()
        
        orthCasePatterns = set([tuple(interp.orthCasePattern) for interp in interpsList])
        lemmaCasePatterns = set([tuple(interp.encodedForm.casePattern) for interp in interpsList])
        prefixCuts = set([interp.encodedForm.prefixCutLength for interp in interpsList])
        
        encodedInterpsList.append(self._encodeCompressByte(orthCasePatterns, lemmaCasePatterns, prefixCuts))
        
        if not self._casePatternsAreEncodedInCompressByte(orthCasePatterns):
            minOrthCasePatterns = self._getMinOrthCasePatterns(interpsList)
            encodedInterpsList.append(len(minOrthCasePatterns))
            for casePattern in minOrthCasePatterns:
                encodedInterpsList.extend(self._encodeCasePattern(casePattern))
        
        for interp in sorted(interpsList, key=lambda i: i.getSortKey()):
            if not self._casePatternsAreEncodedInCompressByte(orthCasePatterns):
                encodedInterpsList.extend(self._encodeCasePattern(interp.orthCasePattern))
            if not self._prefixCutsAreEncodedInCompressByte(prefixCuts):
                encodedInterpsList.append(interp.encodedForm.prefixCutLength)
            encodedInterpsList.append(interp.encodedForm.cutLength)
            encodedInterpsList.extend(serializeString(interp.encodedForm.suffixToAdd))
            if not self._casePatternsAreEncodedInCompressByte(lemmaCasePatterns):
                encodedInterpsList.extend(self._encodeCasePattern(interp.encodedForm.casePattern))
            encodedInterpsList.extend(htons(interp.tagnum))
            encodedInterpsList.append(interp.namenum)
            encodedInterpsList.extend(htons(interp.qualifiers))
        
        res.extend(htons(len(encodedInterpsList)))
        res.extend(encodedInterpsList)
        return res

class Encoder4Generator(Encoder):
    
    def __init__(self, encoding='utf8'):
        super(Encoder4Generator, self).__init__(False, encoding)
    
    def encodeData(self, interpsList):
        return self._doEncodeData(interpsList)
    
    def _encodeInterps4Type(self, typenum, interpsList):
        res = bytearray()
        res.extend(self._encodeTypeNum(typenum))
        encodedInterpsList = bytearray()
        for interp in sorted(interpsList, key=lambda i: i.getSortKey()):
            encodedInterpsList.extend(serializeString(interp.homonymId))
            encodedInterpsList.extend(serializeString(interp.encodedForm.prefixToAdd))
            encodedInterpsList.append(interp.encodedForm.cutLength)
            encodedInterpsList.extend(serializeString(interp.encodedForm.suffixToAdd))
            encodedInterpsList.extend(htons(interp.tagnum))
            encodedInterpsList.append(interp.namenum)
            encodedInterpsList.extend(htons(interp.qualifiers))
        
        res.extend(htons(len(encodedInterpsList)))
        res.extend(encodedInterpsList)
        return res
