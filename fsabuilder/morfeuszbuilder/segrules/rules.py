'''
Created on 24 sty 2014

@author: mlenart
'''

import copy
from morfeuszbuilder.segrules.rulesNFA import RulesNFAState
from morfeuszbuilder.utils.exceptions import ConfigFileException

class SegmentRule(object):

    def __init__(self, linenum):
        
        self.weak = False
        self.linenum = linenum
        self.autogenerated = False
    
    def setWeak(self, weak):
        self.weak = weak
        return self
    
    def addToNFA(self, fsa):
        raise NotImplementedError()
    
    def allowsEmptySequence(self):
        raise NotImplementedError()
    
    def _doAddToNFA(self, startStates, endState):
        raise NotImplementedError()
    
    def transformToGeneratorVersion(self):
        raise NotImplementedError()
    
    def isSinkRule(self):
        return False
    
    def isShiftOrthRule(self):
        raise NotImplementedError()

    def getAtomicRules(self):
        raise NotImplementedError()

    def getAdditionalAtomicRules4Generator(self):
        raise NotImplementedError()
    
    def makeShiftOrthRule(self):
        pass

    def __repr__(self):
        return str(self)

    def validate(self, filename):
        pass

class TagRule(SegmentRule):
    
    def __init__(self, segnum, shiftOrth, segtype, linenum, weak=False):
        self.segnum = segnum
        self.segtype = segtype
        self.shiftOrth = shiftOrth
        self.linenum = linenum
        self.weak = weak
        self.autogenerated = False

    def addToNFA(self, fsa):
        endState = RulesNFAState(self, final=True, weak=self.weak, autogenerated=self.autogenerated)
        self._doAddToNFA(fsa.initialState, endState)
    
    def _doAddToNFA(self, startState, endState):
        startState.addTransition((self.segnum, self.shiftOrth), endState)
    
    def allowsEmptySequence(self):
        return False
    
    def __str__(self):
        res = self.segtype
        # res += '(' + str(self.segnum) + ')'
        if self.shiftOrth:
            res += '>'
        return res
#         return u'%s(%d)' % (self.segtype, self.segnum)
    
    def transformToGeneratorVersion(self):
        return copy.deepcopy(self)
    
    def isShiftOrthRule(self):
        return self.shiftOrth
    
    def makeShiftOrthRule(self):
        self.shiftOrth = True

    def getAtomicRules(self):
        yield self

    def getAdditionalAtomicRules4Generator(self):
        res = [ copy.deepcopy(self) ]
        res[0].autogenerated = True
        return res

class UnaryRule(SegmentRule):
    
    def __init__(self, child, linenum):
        super(UnaryRule, self).__init__(linenum)
        self.child = child
        assert not child.isSinkRule()
    
    def isShiftOrthRule(self):
        return self.child.isShiftOrthRule()
    
    def makeShiftOrthRule(self):
        self.child.makeShiftOrthRule()

    def getAtomicRules(self):
        for leaf in self.child.getAtomicRules():
            yield leaf

    def getAdditionalAtomicRules4Generator(self):
        return self.child.getAdditionalAtomicRules4Generator()

    def validate(self, filename):
        self.child.validate(filename)

class ComplexRule(SegmentRule):
    
    def __init__(self, children, linenum):
        super(ComplexRule, self).__init__(linenum)
        self.children = children
        assert not any(map(lambda c: c.isSinkRule(), children))
    
    def addToNFA(self, fsa):
        endState = RulesNFAState(self, final=True, weak=self.weak, autogenerated=self.autogenerated)
        self._doAddToNFA(fsa.initialState, endState)

    def getAtomicRules(self):
        for child in self.children:
            for leaf in child.getAtomicRules():
                yield leaf
    
    def makeShiftOrthRule(self):
        for child in self.children:
            child.makeShiftOrthRule()

class ConcatRule(ComplexRule):
    
    def __init__(self, children, linenum):
        super(ConcatRule, self).__init__(children, linenum)
        assert type(children) == list

    def _doAddToNFA(self, startState, endState):
        currStartState = startState
        for child in self.children[:-1]:
            currEndState = RulesNFAState(self)
            child._doAddToNFA(currStartState, currEndState)
            nextStartState = RulesNFAState(self)
            currEndState.addTransition(None, nextStartState)
            currStartState = nextStartState
        lastChild = self.children[-1]
        lastChild._doAddToNFA(currStartState, endState)
    
    def allowsEmptySequence(self):
        return all(map(lambda rule: rule.allowsEmptySequence(), self.children))
    
    def __str__(self):
        return u' '.join(map(lambda c: str(c), self.children))
    
    def isShiftOrthRule(self):
        return all(map(lambda c: c.isShiftOrthRule(), self.children))
    
    def transformToGeneratorVersion(self):
        newChildren = [child.transformToGeneratorVersion() for child in self.children if not child.allowsEmptySequence() or child.isShiftOrthRule()]
        if newChildren == []:
            return SinkRule()
        hasNonOptionalNonShiftingRule = False
        for child in newChildren:
#             print 'child=', child
            if child.isSinkRule() or hasNonOptionalNonShiftingRule:
                return SinkRule()
            elif not child.isShiftOrthRule():
                hasNonOptionalNonShiftingRule = True
#                 print 'got nonshifting'
        res = ConcatRule(newChildren, self.linenum)
        res.setWeak(self.weak)
        return res

    def getAdditionalAtomicRules4Generator(self):
        res = []
        currShiftOrthRule = None
        for rule in list(self.children):
            if rule.isShiftOrthRule():
                if currShiftOrthRule:
                    currShiftOrthRule = ConcatRule([currShiftOrthRule, rule], rule.linenum)
                else:
                    currShiftOrthRule = rule
            else:
                for atomicRule in rule.getAdditionalAtomicRules4Generator():
                    if currShiftOrthRule:
                        res.append(ConcatRule([currShiftOrthRule, atomicRule], atomicRule.linenum))
                    else:
                        res.append(atomicRule)
                currShiftOrthRule = None
        for rule in res:
            rule.autogenerated = True
        return res

    def validate(self, filename):
        for rule in self.children:
            rule.validate(filename)
            if self.children[-1].isShiftOrthRule() \
                    and not all(map(lambda c: c.isShiftOrthRule(), self.children)):
                raise ConfigFileException(
                    filename,
                    self.linenum,
                    u'If the rightmost subrule of concatenation "%s" is with ">", than all subrules must be with ">"' % str(self))

class OrRule(ComplexRule):
    
    def __init__(self, children, linenum):
        super(OrRule, self).__init__(children, linenum)
    
    def _doAddToNFA(self, startState, endState):
        for child in self.children:
            intermStartState = RulesNFAState(self)
            intermEndState = RulesNFAState(self)
            startState.addTransition(None, intermStartState)
            child._doAddToNFA(intermStartState, intermEndState)
            intermEndState.addTransition(None, endState)
    
    def allowsEmptySequence(self):
        return any(map(lambda rule: rule.allowsEmptySequence(), self.children))
    
    def __str__(self):
        return u' | '.join(map(lambda c: str(c), self.children))
    
    def isShiftOrthRule(self):
        return all(map(lambda c: c.isShiftOrthRule(), self.children))
    
    def transformToGeneratorVersion(self):
        newChildren = [child.transformToGeneratorVersion() for child in self.children if not child.allowsEmptySequence() or child.isShiftOrthRule()]
        newChildren = filter(lambda c: not c.isSinkRule(), newChildren)
        if newChildren == []:
            return SinkRule()
        else:
            res = OrRule(newChildren, self.linenum)
            res.setWeak(self.weak)
            return res

    def getAdditionalAtomicRules4Generator(self):
        res = []
        for rule in self.children:
            res.extend(rule.getAdditionalAtomicRules4Generator())
        return res

    def validate(self, filename):
        for rule in self.children:
            rule.validate(filename)
            if not (
                    all(map(lambda c: c.isShiftOrthRule(), self.children))
                    or not any(map(lambda c: c.isShiftOrthRule(), self.children))):
                raise ConfigFileException(
                    filename,
                    self.linenum,
                    u'All subrules of alternative "%s" must be either with or without ">"' % str(self))
    
class ZeroOrMoreRule(UnaryRule):
    
    def __init__(self, child, linenum):
        super(ZeroOrMoreRule, self).__init__(child, linenum)
        assert isinstance(child, SegmentRule)
    
    def addToNFA(self, fsa):
        raise ValueError()
    
    def _doAddToNFA(self, startState, endState):
        intermStartState = RulesNFAState(self)
        intermEndState = RulesNFAState(self)
        
        startState.addTransition(None, intermStartState)
        startState.addTransition(None, endState)
        self.child._doAddToNFA(intermStartState, intermEndState)
        intermEndState.addTransition(None, endState)
        intermEndState.addTransition(None, intermStartState)
    
    def allowsEmptySequence(self):
        return True
    
    def transformToGeneratorVersion(self):
        if self.isShiftOrthRule():
            return copy.deepcopy(self)
        else:
            return SinkRule()
    
    def __str__(self):
        return u'(' + str(self.child) + ')*'

class OptionalRule(UnaryRule):
    
    def __init__(self, child, linenum):
        super(OptionalRule, self).__init__(child, linenum)
        assert isinstance(child, SegmentRule)
    
    def addToNFA(self, fsa):
        raise ValueError()
    
    def _doAddToNFA(self, startState, endState):
        intermStartState = RulesNFAState(self)
        intermEndState = RulesNFAState(self)
        
        startState.addTransition(None, intermStartState)
        startState.addTransition(None, endState)
        self.child._doAddToNFA(intermStartState, intermEndState)
        intermEndState.addTransition(None, endState)
    
    def allowsEmptySequence(self):
        return True
    
    def transformToGeneratorVersion(self):
        if self.isShiftOrthRule():
            return copy.deepcopy(self)
        else:
            return self.child.transformToGeneratorVersion()
    
    def __str__(self):
        return u'(' + str(self.child) + ')?'

class SinkRule(SegmentRule):
    
    def __init__(self):
        super(SinkRule, self).__init__(None)
    
    def addToNFA(self, fsa):
        return
    
    def allowsEmptySequence(self):
        return False
    
    def _doAddToNFA(self, startStates, endState):
        return
    
    def transformToGeneratorVersion(self):
        return self
    
    def isSinkRule(self):
        return True
    
    def __str__(self):
        return '<<REMOVED>>'

    def getAdditionalAtomicRules4Generator(self):
        return []