'''
Created on 17 lut 2014

@author: mlenart
'''
import re
import logging
import itertools
from morfeuszbuilder.utils import exceptions

def _getLemmaHomonymPair(lemma):
    if lemma is None:
        return (None, None)
    elif u':' in lemma:
        if lemma.replace(u':', '') == '':
            return (lemma, None)
        else:
            return lemma.split(u':', 1)
    else:
        return (lemma, None)

class Segtypes(object):
    
    def __init__(self, tagset, namesMap, labelsMap, segrulesConfigFile):
        
        self.tagset = tagset
        self.namesMap = namesMap
        self.labelsMap = labelsMap
        self._reverseLabelsMap = dict([(v, k) for (k, v) in labelsMap.iteritems()])
        
        self.filename = segrulesConfigFile.filename
        
        self.segtypes = []
        # self.segtype2Segnum = {}
        # self.segnum2Segtype = {}
        self.patternsList = []

        # (lemma, tagnum) -> [homonym, namenum, labelsnum, segnum]
        self._segnumsMap = {}

        # self._tagnum2Segnum = {}
        # self._lemmaTagnum2Segnum = {}
        
        self._readSegtypes(segrulesConfigFile)
        self._readLexemes(segrulesConfigFile)
        self._readTags(segrulesConfigFile)
        self._indexSegnums()
        
#         print self._lemmaTagnum2Segnum
#         print self._tagnum2Segnum
        logging.info('segment number -> segment type')
        logging.info('------------------------------')
        logging.info(dict(enumerate(self.segtypes)))
        logging.info('------------------------------')
        
    def _validate(self, msg, lineNum, cond):
        if not cond:
            raise exceptions.ConfigFileException(self.filename, lineNum, msg)
    
    def _readSegtypes(self, segrulesConfigFile):
        for lineNum, line in segrulesConfigFile.enumerateLinesInSection('segment types'):
            assert type(line) == unicode
            self._validate(
                           u'Segment type must be a single word', 
                           lineNum,
                           re.match(r'^\w+$', line))
            self._validate(
                           u'Segment type already defined: "%s"' % line, 
                           lineNum,
                           line not in self.segtypes)
            self.segtypes.append(line)

    def _readTags(self, segrulesConfigFile):
        gotWildcardPattern = False
        for lineNum, line in segrulesConfigFile.enumerateLinesInSection('tags'):
            self._parsePattern(lineNum, line, withLemma=False)
            self._validate(
                u'Pattern that matches everything must be the last one',
                lineNum - 1,
                not gotWildcardPattern)
            gotWildcardPattern = gotWildcardPattern or self.patternsList[-1].isWildcardPattern()

        self._validate(
            u'There must be a pattern that matches everything at the end of [tags] section',
            lineNum,
            self.patternsList[-1].isWildcardPattern())
    
    def _readLexemes(self, segrulesConfigFile):
        for lineNum, line in segrulesConfigFile.enumerateLinesInSection('lexemes'):
            self._parsePattern(lineNum, line, withLemma=True)

    def _parseAdditionalConstraints(self, lineNum, fields):
        res = {}
        for f in fields:
            match = re.match(r'(name|labels)=([\S]+)', f, re.U)
            self._validate(
                        u'invalid name or labels constraint: "%s"' % f,
                        lineNum,
                        match)
            key = match.group(1)
            value = match.group(2)
            self._validate(
                u'%s already specified' % key,
                lineNum,
                key not in res)
            if key == 'labels':
                if value:
                    value = frozenset(value.split(u'|'))
                else:
                    value = frozenset()
            res[key] = value
        return res

    def _parsePattern(self, lineNum, line, withLemma):
        split = re.split(r'\s+', line.strip())
        if withLemma:
            self._validate(
                u'Line in [lexemes] section must contain 3 to 5 fields - segment type, lemma, tag pattern and optional constraints on name and labels',
                lineNum,
                len(split) in [3, 4, 5])
            segtype = split[0]
            lemma = split[1]
            pattern = split[2]
            additionalConstraints = self._parseAdditionalConstraints(lineNum, split[3:])
        else:
            self._validate(
                u'Line in [tags] section must contain 2 to 4 fields - segment type, tag pattern and optional constraints on name and labels',
                lineNum,
                len(split) in [2, 3, 4])
            segtype = split[0]
            lemma = None
            pattern = split[1]
            additionalConstraints = self._parseAdditionalConstraints(lineNum, split[2:])
        self._validate(
            u'Undeclared segment type: "%s"' % segtype,
            lineNum,
            segtype in self.segtypes)
        segnum = self.segtypes.index(segtype)

        self._validate(
            u'Pattern must contain only ":", "%", "." and lowercase alphanumeric letters',
            lineNum,
            re.match(r'[a-z_\.\:\%]+', pattern))

        segtypePattern = SegtypePattern(
            lemma,
            pattern,
            additionalConstraints.get('name', u''),
            additionalConstraints.get('labels', frozenset()),
            segnum)
        # print 'segtypePattern', repr(str(segtypePattern))
        self._validate(
            u'There is no tag that matches pattern "%s".' % (pattern),
            lineNum,
            any([segtypePattern.tryToMatchTag(tag) != -1 for tag in self.tagset.getAllTags()]))
        self.patternsList.append(segtypePattern)

    def _getAllExistingLabelsnumCombinations(self, labels):
        if labels:
            for labelsCombination, labelsnum in self.labelsMap.iteritems():
                if labels <= labelsCombination:
                    yield labelsnum
        else:
            yield 0

    def _indexOnePattern(self, p):

        for tag in self.tagset.getAllTags():
            segnum = p.tryToMatchTag(tag)
            if segnum != -1:
                tagnum = self.tagset.getTagnum4Tag(tag)
                self._segnumsMap.setdefault((p.lemma, tagnum), [])
                namenum = self.namesMap.get(p.name, -1)
                for labelsnum in self._getAllExistingLabelsnumCombinations(p.labels):
                    self._segnumsMap[(p.lemma, tagnum)].append((p.homonym, namenum, labelsnum, segnum))

    def _indexSegnums(self):
        logging.info('indexing segment type numbers...')

        # index lexemes
        for p in self.patternsList:
            self._indexOnePattern(p)
        #~ print 'DEBUG', self._segnumsMap[(None, 23)]
        # logging.info(self._segnumsMap)
    
    def hasSegtype(self, segTypeString):
        # return segTypeString in self.segtype2Segnum
        return segTypeString in self.segtypes
    
    def getSegnum4Segtype(self, segTypeString):
        return self.segtypes.index(segTypeString)
        # return self.segtype2Segnum[segTypeString]
    
    def lexeme2Segnum(self, lemma, tagnum, namenum, labelsnum):
        lemma, homonym = _getLemmaHomonymPair(lemma)
        if (lemma, tagnum) in self._segnumsMap:
            for (h, n, l, segnum) in self._segnumsMap[(lemma, tagnum)]:
                if h == homonym:
                    if (n, l) == (namenum, labelsnum) \
                            or (n, l) == (0, 0) \
                            or (n == 0 and l == labelsnum) \
                            or (l == 0 and n == namenum):
                        return segnum
        if homonym is not None:
            return self.lexeme2Segnum(lemma, tagnum, namenum, labelsnum)
        elif lemma is not None:
            return self.lexeme2Segnum(None, tagnum, namenum, labelsnum)
        else:
            assert False

    def getMaxSegnum(self):
        return len(self.segtypes) - 1
    
class SegtypePattern(object):
    
    def __init__(self, lemma, pattern, name, labels, segnum):
        self.lemma = _getLemmaHomonymPair(lemma)[0]
        self.homonym = _getLemmaHomonymPair(lemma)[1]
        self.pattern = pattern
        self.name = name
        self.labels = labels
        self.segnum = segnum

    def tryToMatchTag(self, tag):
        patterns2Match = []
        patterns2Match.append(self.pattern.replace('%', '.*'))
        patterns2Match.append(re.sub(r'\:\%$', '', self.pattern).replace('%', '.*'))

        if any([re.match('^'+p+'$', tag) for p in patterns2Match]):
            return self.segnum
        else:
            return -1

    def isWildcardPattern(self):
        return (self.lemma, self.pattern, self.name, self.labels) == (None, '%', u'', frozenset())

    def __str__(self):
        return u'%s %s %s %s -> %d' % (self.lemma, self.pattern, self.name, self.labels, self.segnum)
