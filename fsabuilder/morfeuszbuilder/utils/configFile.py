'''
Created on 18 lut 2014

@author: mlenart
'''

import re
import codecs
import exceptions

def getHeaderValue(line, lineNum):
    m = re.match(ur'\s*\[(.*?)\]\s*(\#.*)?', line)
    if m:
        return m.group(1)
    else:
        return None

class ConfigFile(object):
    
    def __init__(self, filename, sectionNames):
        self.filename = filename
        self.sectionNames = sectionNames
        self.section2Lines = {}
        self.currSection = None
        self._parse()
    
    def _addSectionStart(self, sectionName, lineNum):
        if not sectionName in self.sectionNames:
            raise exceptions.ConfigFileException(self.filename, lineNum, 'Invalid section: %s' % sectionName)
        if sectionName in self.section2Lines:
            raise exceptions.ConfigFileException(self.filename, lineNum, 'Duplicate section: %s' % sectionName)
        self.section2Lines[sectionName] = []
        self.currSection = sectionName
    
    def _addLine(self, line, lineNum):
        line = line.strip()
        if line:
            if self.currSection is None and not line.startswith('#'):
                raise exceptions.ConfigFileException(self.filename, lineNum, 'Text outside of any section')
            self.section2Lines[self.currSection].append((lineNum, line))
    
    def _getHeaderValue(self, line, lineNum):
        m = re.match(ur'\s*\[(.*?)\]\s*(\#.*)?', line)
        if m:
            return m.group(1)
        else:
            return None
    
    def enumerateLinesInSection(self, sectionName, ignoreComments=True):
        if sectionName not in self.section2Lines:
            raise exceptions.ConfigFileException(self.filename, None, u'Missing section: "%s"' % sectionName)
        if not ignoreComments:
            return self.section2Lines[sectionName]
        else:
            return [(linenum, line) for (linenum, line) in self.section2Lines[sectionName] if not line.startswith('#')]
    
    def _parse(self):
        with codecs.open(self.filename, 'r', 'utf8') as f:
            for lineNum, line in enumerate(f, start=1):
                header = self._getHeaderValue(line, lineNum)
                if header:
                    self._addSectionStart(header, lineNum)
                else:
                    self._addLine(line, lineNum)
