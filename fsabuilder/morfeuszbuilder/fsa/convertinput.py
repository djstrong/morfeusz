'''
Created on Oct 23, 2013

@author: mlenart
'''
import logging
from common import Interpretation4Analyzer
from morfeuszbuilder.fsa.common import Interpretation4Generator
#from morfeuszbuilder.fsa import externalsort

def _mergeEntries(inputLines, lowercase):
    prevKey = None
    prevInterps = None
    for key, interp in inputLines:
        key = key.lower() if lowercase else key
#         print 'key=', key, 'interp=', interp
#         if not key:
#             print 'key=', key, 'interp=', interp
#             assert key
        assert key
        if prevKey and prevKey == key:
            prevInterps.append(interp)
        else:
            if prevKey:
                yield (prevKey, frozenset(prevInterps))
            prevKey = key
            prevInterps = [interp]
    if prevInterps:
        yield (prevKey, frozenset(prevInterps))

class LineParser(object):

    def __init__(self):
        self.inCopyright = False

    def ignoreLine(self, line):
        if not line:
            return True
        elif line.strip() == u'#<COPYRIGHT>':
            self.inCopyright = True
            return True
        elif line.strip() == u'#</COPYRIGHT>':
            self.inCopyright = False
            return True
        elif self.inCopyright:
            return True
        elif line and not ' ' in ''.join(line.split('\t')[:2]):
            return False
        elif line.startswith(u'#!DICT-ID'):
            return True
        else:
            logging.warn(u'Ignoring line: "%s" - contains space in text form or lemma' % (line.strip()))
            return True

    def parseLine(self, line):
        splitLine = line.strip().split(u'\t')
        if len(splitLine) == 5:
            orth, base, tag, name, qualifier = splitLine
        elif len(splitLine) == 4:
            orth, base, tag, name = splitLine
            qualifier = ''
        elif len(splitLine) == 3:
            orth, base, tag = splitLine
            name = ''
            qualifier = ''
        else:
            raise ValueError('input line "%s" does not have 3, 4 or 5 tab-separated fields' % line)
        return orth, base, tag, name, qualifier

def parseQualifiers(string):
    if string:
        return frozenset(string.split(u'|'))
    else:
        return frozenset()

class PolimorfConverter4Analyzer(object):
    
    def __init__(self, tagset, namesMap, qualifiersMap, encoder, inputEncoding, segmentRulesManager):
        self.tagset = tagset
        self.namesMap = namesMap
        self.qualifiersMap = qualifiersMap
        self.encoder = encoder
        self.inputEncoding = inputEncoding
        self.segmentRulesManager = segmentRulesManager
    
    # we do it the ugly way (parse to plain text) because it is way more memory-efficient
    def _partiallyParseLines(self, inputLines):
        lineParser = LineParser()
        for line in inputLines:
            line = line.decode(self.inputEncoding).strip('\n')
            if not lineParser.ignoreLine(line):
                orth, base, tag, name, qualifier = lineParser.parseLine(line)
                
                tagnum = self.tagset.getTagnum4Tag(tag)
                namenum = self.namesMap[name]
                qualifiers = parseQualifiers(qualifier)
                qualsnum = self.qualifiersMap[qualifiers]
                typenum = self.segmentRulesManager.lexeme2SegmentTypeNum(base, tagnum, namenum, qualsnum)

                assert not (
                    self.segmentRulesManager.shiftOrthMagic.shouldReplaceLemmaWithOrth(typenum)
                    and self.segmentRulesManager.shiftOrthMagic.getNewSegnum4ShiftOrth(typenum) != None)

                if self.segmentRulesManager.shiftOrthMagic.shouldReplaceLemmaWithOrth(typenum):
                    # print 'replace %s %s %s %d with %s %s %s %d' % (orth, base, tag, typenum, orth, orth, tag, typenum)
                    base = orth

                yield '\t'.join((
                            orth.encode(self.inputEncoding),
                            base.encode(self.inputEncoding),
                            str(tagnum),
                            str(namenum),
                            str(typenum), 
                            str(qualsnum)))

                if self.segmentRulesManager.shiftOrthMagic.getNewSegnum4ShiftOrth(typenum) != None:
                    # print 'add to existing %s %s %s %d also this: %s %s %s %d' % (orth, base, tag, typenum, orth, orth, tag, self.segmentRulesManager.shiftOrthMagic.getNewSegnum4ShiftOrth(typenum))
                    base = orth
                    typenum = self.segmentRulesManager.shiftOrthMagic.getNewSegnum4ShiftOrth(typenum)
                    yield '\t'.join((
                            orth.encode(self.inputEncoding),
                            base.encode(self.inputEncoding),
                            str(tagnum),
                            str(namenum),
                            str(typenum),
                            str(qualsnum)))
    
    # input lines are encoded and partially parsed
    def _sortLines(self, inputLines):
        return sorted(inputLines, key=lambda line: self.encoder.word2SortKey(line.split('\t')[0].decode('utf8')))
#         return sorted(inputLines, key=lambda line: self.encoder.word2SortKey(line.split('\t')[0].decode('utf8')))
    
    def _reallyParseLines(self, inputLines):
        for line in inputLines:
            line = line.decode(self.inputEncoding).strip(u'\n')
            if line:
                orth, base, tagnum, namenum, typenum, qualsnum = line.split(u'\t')
                tagnum = int(tagnum)
                namenum = int(namenum)
                typenum = int(typenum)
                #~ qualifiers = qualifierStr.split('|') if qualifierStr else []
                qualsnum = int(qualsnum)
                yield (orth, Interpretation4Analyzer(orth, base, tagnum, namenum, typenum, qualsnum))

    def convert(self, inputLines):
        return _mergeEntries(self._reallyParseLines(self._sortLines(self._partiallyParseLines(inputLines))), lowercase=True)

class PolimorfConverter4Generator(object):
    
    def __init__(self, tagset, namesMap, qualifiersMap, encoder, inputEncoding, segmentRulesManager):
        self.tagset = tagset
        self.namesMap = namesMap
        self.qualifiersMap = qualifiersMap
        self.encoder = encoder
        self.inputEncoding = inputEncoding
        self.segmentRulesManager = segmentRulesManager
    
    # we do it the ugly way (parse to plain text) because it is way more memory-efficient
    def _partiallyParseLines(self, inputLines):
        lineParser = LineParser()
        for line in inputLines:
            line = line.decode(self.inputEncoding).strip('\n')
            if not lineParser.ignoreLine(line):
                orth, base, tag, name, qualifier = lineParser.parseLine(line)
                if base:
                    homonymId = u''
                    if u':' in base:
                        assumedBase, assumedHomonymId = base.split(u':', 1)
                        if assumedBase != u'' and assumedHomonymId != u'' and assumedHomonymId.isalnum():
                            base, homonymId = assumedBase, assumedHomonymId
                    tagnum = self.tagset.getTagnum4Tag(tag)
                    namenum = self.namesMap[name]
                    qualifiers = parseQualifiers(qualifier)
                    qualsnum = self.qualifiersMap[qualifiers]
                    typenum = self.segmentRulesManager.lexeme2SegmentTypeNum(base, tagnum, namenum, qualsnum)

                    if self.segmentRulesManager.shiftOrthMagic.shouldReplaceLemmaWithOrth(typenum):
                        # print 'replace %s %s %s %d with %s %s %s %d' % (orth, base, tag, typenum, orth, orth, tag, typenum)
                        base = orth

                    yield '\t'.join((
                                orth.encode(self.inputEncoding), 
                                base.encode(self.inputEncoding), 
                                str(tagnum), 
                                str(namenum), 
                                str(typenum),
                                homonymId.encode(self.inputEncoding), 
                                str(qualsnum)))

                    if self.segmentRulesManager.shiftOrthMagic.getNewSegnum4ShiftOrth(typenum) != None:
                        base = orth
                        typenum = self.segmentRulesManager.shiftOrthMagic.getNewSegnum4ShiftOrth(typenum)
                        yield '\t'.join((
                                orth.encode(self.inputEncoding),
                                base.encode(self.inputEncoding),
                                str(tagnum),
                                str(namenum),
                                str(typenum),
                                homonymId.encode(self.inputEncoding),
                                str(qualsnum)))
                else:
                    logging.warn('Ignoring line: "%s" - contains empty lemma', line.strip())
    
    # input lines are encoded and partially parsed
    def _sortLines(self, inputLines):
        return sorted(inputLines, key=lambda line: (self.encoder.word2SortKey(line.split('\t')[1].decode('utf8')), line))
    
    def _reallyParseLines(self, inputLines):
        prevLine = None
        for line in inputLines:
            line = line.decode(self.inputEncoding).strip(u'\n')
            if line and line != prevLine:
                orth, base, tagnum, namenum, typenum, homonymId, qualsnum = line.split(u'\t')
#                 print orth.encode('utf8'), base.encode('utf8'), homonymId
                tagnum = int(tagnum)
                namenum = int(namenum)
                typenum = int(typenum)
                del prevLine
                prevLine = line
                qualsnum = int(qualsnum)
                #~ qualifiers = qualifierStr.split('|') if qualifierStr else ()
                yield (base, Interpretation4Generator(orth, base, tagnum, namenum, typenum, homonymId, qualsnum))
    
    def convert(self, inputLines):
        return _mergeEntries(self._reallyParseLines(self._sortLines(self._partiallyParseLines(inputLines))), lowercase=False)
