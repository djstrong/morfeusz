'''
Created on Oct 8, 2013

@author: mlenart
'''

class Register(object):
    '''
    States register.
    '''


    def __init__(self):
        self._map = {}
    
    def addState(self, state):
        self._map[state.getRegisterKey()] = state
    
    def getEquivalentState(self, state):
        return self._map[state.getRegisterKey()]
    
    def containsEquivalentState(self, state):
        return state.getRegisterKey() in self._map
    
    def getStatesNum(self):
        return len(self._map)
