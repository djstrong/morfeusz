'''
Created on 12 mar 2014

@author: mlenart
'''
import logging
from morfeuszbuilder.fsa import state
from morfeuszbuilder.utils.serializationUtils import htons
from morfeuszbuilder.utils import exceptions

MAX_FSA_SIZE = 65535

class RulesState(state.State):
    
    def __init__(self):
        super(RulesState, self).__init__()
        self.weak = None
    
    def setAsAccepting(self, weak):
        self.weak = weak
        self.encodedData = bytearray([1 if weak else 0])
    
    def getEncodedSize(self):
        stateInfoSize = 2 # accepting info + transitionsNum
        transitionsSize = 4 * len(self.transitionsMap)
        return stateInfoSize + transitionsSize

class RulesFSA(object):
    
    def __init__(self):
        self.initialState = state.State()
        self.ACCEPTING_FLAG = 1
        self.WEAK_FLAG = 2
    
    def stateData2bytearray(self, state):
        res = bytearray()
        firstByte = 0
        if state.isAccepting():
            firstByte |= self.ACCEPTING_FLAG
        if state.weak:
            firstByte |= self.WEAK_FLAG
        res.append(firstByte)
        
        secondByte = len(state.transitionsMap)
        res.append(secondByte)
        
        return res
    
    def transitionsData2bytearray(self, state):
        res = bytearray()
#         logging.debug('next')
        for (segnum, shiftOrth), nextState in sorted(state.transitionsMap.iteritems()):
            res.append(segnum)
            if shiftOrth:
                res.append(1)
            else:
                res.append(0)
            offset = nextState.offset
            exceptions.validate(offset <= MAX_FSA_SIZE,
                                u'Segmentation rules are too big and complicated' \
                                + u'- the resulting automaton would exceed its max size which is %d' \
                                % MAX_FSA_SIZE)
            res.extend(htons(offset))
        return res
    
    def serialize(self):
        self.initialState.calculateOffsets(sizeCounter=lambda s: s.getEncodedSize())
        res = bytearray()
        
        for state in sorted(self.initialState.dfs(set()), key=lambda s: s.offset):
            res.extend(self.stateData2bytearray(state))
            res.extend(self.transitionsData2bytearray(state))
        
        return res
