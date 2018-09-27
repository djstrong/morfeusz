'''
Created on 12 mar 2014

@author: mlenart
'''

from pyparsing import ParseException
from morfeuszbuilder.utils import exceptions

def pyparseString(rule, lineNum, line, filename):
    try:
        return rule.parseString(line, parseAll=True)
    except ParseException as ex:
        msg = u'%s:%d - Preprocessing of segmentation rules failed.\n' % (filename, lineNum)
        msg += line + '\n'
        msg += (ex.col - 1) * ' ' + '^\n'
        msg += ex.msg
#             print unicode(exceptions.SegtypesException(msg)).encode('utf8')
        raise exceptions.SegtypesException(msg)