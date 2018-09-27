'''
Created on 17 lut 2014

@author: mlenart
'''

import codecs
import re
from morfeuszbuilder.utils.exceptions import FSABuilderException

class Tagset(object):
    
    TAGS = 1
    NAMES = 2
    SEP = '\t'
    
    def __init__(self, filename=None, encoding='utf8'):
        self.tagsetId = None
        self.tag2tagnum = {}
        #~ self._name2namenum = {}
        if filename:
            self._doInit(filename, encoding)
        self._tagnum2tag = dict(map(lambda (k, v): (v, k), self.tag2tagnum.iteritems()))
    
    def _doInit(self, filename, encoding):
        insideTags = False
        with codecs.open(filename, 'r', encoding) as f:
            for linenum, line in enumerate(f, start=1):
                line = line.strip('\n\r')
                if linenum == 1:
                    match = re.match(r'\#\!TAGSET-ID\s+(.*)$', line)
                    if match:
                        self.tagsetId = match.group(1)
                    else:
                        raise FSABuilderException('missing TAGSET-ID in first line of tagset file')
                elif line == u'[TAGS]':
                    insideTags = True
                #~ elif line == u'[NAMES]':
                    #~ addingTo = Tagset.NAMES
                elif line and not line.startswith(u'#'):
                    if not insideTags:
                        raise FSABuilderException('"%s" - text outside [TAGS] section in tagset file line %d' % (line, linenum))
                    res = self.tag2tagnum
                    if len(line.split(Tagset.SEP)) != 2:
                        raise FSABuilderException('"%s" - invalid line %d' % (line, linenum))
                    tagNum = line.split(Tagset.SEP)[0]
                    tag = line.split(Tagset.SEP)[1]
                    if tag in res:
                        raise FSABuilderException('duplicate tag: "%s"' % tag)
                    if int(tagNum) in res.values():
                        raise FSABuilderException('line %d: tagId %d assigned for tag "%s" already appeared somewhere else.' % (linenum, int(tagNum), tag))
                    res[tag] = int(tagNum)
    
    def getAllTags(self):
        return self.tag2tagnum.keys()
    
    def getTagnum4Tag(self, tag):
        if tag in self.tag2tagnum:
            return self.tag2tagnum[tag]
        else:
            raise FSABuilderException('invalid tag: "%s"' % tag)
    
    def getTag4Tagnum(self, tagnum):
        return self._tagnum2tag[tagnum]
