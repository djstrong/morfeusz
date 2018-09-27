#-*- coding: utf-8 -*-
'''
Created on Oct 8, 2013

@author: mlenart
'''
import unittest
import os
from morfeuszbuilder.fsa import fsa, visualizer, encode
from morfeuszbuilder.fsa.serializer import SimpleSerializer

class Test(unittest.TestCase):
    pass
#     def testSimpleConstruction(self):
#         a = fsa.FSA(encode.SimpleEncoder())
#         input = sorted([
#                 (u'bić', ''),
#                 (u'bij', ''),
#                 (u'biją', ''),
#                 (u'bijcie', ''),
#                 (u'bije', ''),
#                 (u'bijecie', ''),
#                 (u'bijemy', ''),
#                 (u'bijesz', ''),
#                 (u'biję', ''),
#                 (u'bijmy', ''),
#                 (u'bili', 'asd'),
#                 (u'biliby', ''),
#                 (u'bilibyście', ''),
#                 (u'bilibyśmy', ''),
#                 (u'biliście', 'asdfas'),
#                 (u'biliśmy', ''),
#                 (u'bił', 'wersadfas'),
#                 (u'biła', 'asdfasd'),
#                 (u'biłaby', 'asdfa'),
#                 (u'biłabym', ''),
#                 (u'biłabyś', 'asdfa'),
#                 (u'biłam', 'dfas'),
#                 (u'biłaś', 'asdfas'),
#                 (u'biłby', ''),
#                 (u'biłbym', 'asdfa'),
#                 (u'biłbyś', ''),
#                 (u'biłem', ''),
#                 (u'biłeś', 'sadfa'),
#                 (u'biły', ''),
#                 (u'biłyby', ''),
#                 (u'biłybyście', ''),
#                 (u'biłybyśmy', ''),
#                 (u'biłyście', ''),
#                 (u'biłyśmy', ''),
#                 ], key=lambda w: bytearray(w[0], 'utf8'))
#         a.feed(input)
#         for w, res in input:
#             recognized = a.tryToRecognize(w)
#             assert recognized == res
#         a.calculateOffsets(lambda state: 1 + 4 * len(state.transitionsMap.keys()) + (len(state.encodedData) if state.isAccepting() else 0))
#         visualizer.Visualizer().visualize(a)
#     
#     def testPolimorfConstruction(self):
#         inputFile = os.path.join(os.path.dirname(__file__), 'PoliMorfSmall.tab')
#         tagsetFile = os.path.join(os.path.dirname(__file__), 'polimorf.tagset')
#         fsa = buildfsa.buildFromPoliMorf(inputFile, tagsetFile)
#         serializer = SimpleSerializer(fsa)
#         serializer.serialize2BinaryFile('/tmp/test0.fsa')
#         visualizer.Visualizer().visualize(fsa)

if __name__ == "__main__":
    #import sys;sys.argv = ['', 'Test.testSimpleConstruction']
    unittest.main()