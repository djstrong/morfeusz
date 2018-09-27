'''
Created on Nov 7, 2013

@author: mlenart
'''

import codecs
import logging

class EncodedFormWithoutPrefix(object):
    
    def __init__(self, fromWord, targetWord, lowercase):
        assert type(fromWord) == unicode
        assert type(targetWord) == unicode
        root = u''
        for o, b in zip(fromWord, targetWord):
            if ((o.lower() == b.lower()) if lowercase else o == b):
                root += b
            else:
                break
        self.cutLength = len(fromWord) - len(root)
        self.suffixToAdd = targetWord[len(root):]
        self.casePattern = [c == c.upper() and c != c.lower() for c in root]
#         self.prefixCutLength = 0

class EncodedForm4Generator(object):
    
    def __init__(self, fromWord, targetWord):
        assert type(fromWord) == unicode
        assert type(targetWord) == unicode
        bestEncodedForm = None
        bestPrefixLength = -1
        for prefixLength in range(min(len(targetWord), 5)):
            encodedForm = EncodedFormWithoutPrefix(fromWord, targetWord[prefixLength:], lowercase=False)
            if not bestEncodedForm \
            or len(encodedForm.suffixToAdd) + prefixLength < len(bestEncodedForm.suffixToAdd) + bestPrefixLength:
                bestEncodedForm = encodedForm
                bestPrefixLength = prefixLength
        assert bestPrefixLength >= 0
        
        self.cutLength = bestEncodedForm.cutLength
        self.suffixToAdd = bestEncodedForm.suffixToAdd
        self.prefixToAdd = targetWord[:bestPrefixLength]

class EncodedForm4Analyzer(object):
    
    def __init__(self, fromWord, targetWord):
        assert type(fromWord) == unicode
        assert type(targetWord) == unicode
        bestEncodedForm = None
        bestPrefixCutLength = -1
        for prefixCutLength in range(min(len(fromWord), 5)):
            encodedForm = EncodedFormWithoutPrefix(fromWord[prefixCutLength:], targetWord, lowercase=True)
            if not bestEncodedForm \
            or len(encodedForm.suffixToAdd) + prefixCutLength < len(bestEncodedForm.suffixToAdd):
                bestEncodedForm = encodedForm
                bestPrefixCutLength = prefixCutLength
        assert bestPrefixCutLength >= 0
        
        self.prefixCutLength = bestPrefixCutLength
        self.cutLength = bestEncodedForm.cutLength
        self.suffixToAdd = bestEncodedForm.suffixToAdd
        self.casePattern = bestEncodedForm.casePattern
        
class Interpretation4Analyzer(object):
    
    def __init__(self, orth, base, tagnum, namenum, typenum, qualifiers):
        self.encodedForm = EncodedForm4Analyzer(orth, base)
        self.orthCasePattern = [c == c.upper() and c != c.lower() for c in orth[:len(orth) - self.encodedForm.cutLength]]
        self.tagnum = tagnum
        self.namenum = namenum
        self.typenum = typenum
        self.qualifiers = qualifiers
    
    def getSortKey(self):
        return (
                self.encodedForm.cutLength,
                self.encodedForm.prefixCutLength,
                tuple(self.encodedForm.suffixToAdd), 
                tuple(self.encodedForm.casePattern),
                tuple(self.orthCasePattern),
                self.tagnum, 
                self.namenum,
                self.typenum)
    
    def __eq__(self, other):
        if isinstance(other, Interpretation4Analyzer):
            return self.getSortKey() == other.getSortKey()
        else:
            return False
    
    def __hash__(self):
        return hash(self.getSortKey())

class Interpretation4Generator(object):
    
    def __init__(self, orth, base, tagnum, namenum, typenum, homonymId, qualifiers):
        self.lemma = base
        self.encodedForm = EncodedForm4Generator(base, orth)
        self.tagnum = tagnum
        self.namenum = namenum
        self.typenum = typenum
        self.homonymId = homonymId
        self.qualifiers = qualifiers
    
    def getSortKey(self):
        return (
                self.homonymId,
                self.tagnum,
                self.encodedForm.cutLength, 
                tuple(self.encodedForm.suffixToAdd),
#                 tuple(self.encodedForm.casePattern), 
                self.namenum,
                self.typenum)
    
    def __eq__(self, other):
        if isinstance(other, Interpretation4Generator):
            return self.getSortKey() == other.getSortKey()
        else:
            return False
    
    def __hash__(self):
        return hash(self.getSortKey())
    
    def __unicode__(self):
        return u'<%s,(%d %s),%d,%d>' % (self.lemma.decode('utf8'), self.encodedForm.cutLength, self.encodedForm.suffixToAdd.decode('utf8'), self.tagnum, self.namenum)
    
    def __repr__(self):
        return unicode(self)
