
import logging

class ShiftOrthMagic(object):

    def __init__(self):
        # segnum -> newSegnum
        # used to add (orth, orth, newSegnum) to input dictionary for each (orth, lemma, segnum) entry
        self._bothShiftAndNonShiftSegnums = {}

        # list of segnums
        # used to replace each (orth, lemma, segnum) with (orth, orth, segnum) in input dictionary
        self._onlyShiftSegnums = set()

    def shouldReplaceLemmaWithOrth(self, typenum):
        return typenum in self._onlyShiftSegnums

    def getNewSegnum4ShiftOrth(self, typenum):
        return self._bothShiftAndNonShiftSegnums.get(typenum, None)

    def doShiftOrthMagic(self, resultsMap, segtypesHelper, shiftOrthSegtypes, nonShiftOrthSegtypes):

        nextNewSegnum = segtypesHelper.getMaxSegnum() + 1
        additionalIdsMap = {}

        for segtype in (shiftOrthSegtypes & nonShiftOrthSegtypes):
            oldSegnum = segtypesHelper.getSegnum4Segtype(segtype)
            if not oldSegnum in self._bothShiftAndNonShiftSegnums:
                self._bothShiftAndNonShiftSegnums[oldSegnum] = nextNewSegnum
                additionalIdsMap[nextNewSegnum] = (segtype + '>')
                nextNewSegnum += 1

        logging.info('segment number -> additional segment type (with ">")')
        logging.info(str(additionalIdsMap))

        for segtype in shiftOrthSegtypes - nonShiftOrthSegtypes:
            self._onlyShiftSegnums.add(segtypesHelper.getSegnum4Segtype(segtype))

        for _, rules in resultsMap.iteritems():
            for rule in rules:
                for atomicRule in rule.getAtomicRules():
                    if atomicRule.segnum in self._bothShiftAndNonShiftSegnums and atomicRule.shiftOrth:
                        # print 'replace segnum %d -> %d' % (atomicRule.segnum, self._bothShiftAndNonShiftSegnums[atomicRule.segnum])
                        atomicRule.segnum = self._bothShiftAndNonShiftSegnums[atomicRule.segnum]
