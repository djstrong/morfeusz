

#include <vector>
#include <cassert>
#include "SegrulesFSA.hpp"

using namespace std;

namespace morfeusz {

    void SegrulesFSA::proceedToNext(
            const unsigned char segnum,
            const SegrulesState& state,
            bool atEndOfWord,
            SegrulesState& resState) const {
        assert(!state.failed);
        if (state.offset == 0) {
            doProceedFromInitialState(segnum, atEndOfWord, resState);
        } else {
            doProceedFromNonInitialState(segnum, state, atEndOfWord, resState);
        }
    }

    void SegrulesFSA::doProceedFromInitialState(
            const unsigned char segnum,
            bool atEndOfWord,
            SegrulesState& resState) const {
        const SegrulesState& newState = initialTransitions[segnum];
        if ((atEndOfWord && newState.accepting)
                || (!atEndOfWord && !newState.sink)) {
            resState = newState;
        }
    }

    void SegrulesFSA::doProceedFromNonInitialState(
            const unsigned char segnum,
            const SegrulesState& state,
            bool atEndOfWord,
            SegrulesState& resState) const {
        const unsigned char* currPtr = ptr + state.offset + 1;
        const unsigned char transitionsNum = *currPtr++;
        for (int i = 0; i < transitionsNum; i++) {
            if (*currPtr == segnum) {
                SegrulesState newState = this->transition2State(currPtr);
                if ((atEndOfWord && newState.accepting)
                        || (!atEndOfWord && !newState.sink)) {
                    resState = newState;
                }
            }
            currPtr += 4;
        }
    }

    SegrulesState SegrulesFSA::transition2State(const unsigned char* transitionPtr) const {
        unsigned char ACCEPTING_FLAG = 1;
        unsigned char WEAK_FLAG = 2;
        SegrulesState res;
        transitionPtr++;
        res.shiftOrthFromPrevious = *transitionPtr++;
        res.offset = readInt16(transitionPtr);
        res.accepting = *(ptr + res.offset) & ACCEPTING_FLAG;
        res.weak = *(ptr + res.offset) & WEAK_FLAG;
        res.sink = *(ptr + res.offset + 1) == 0;
        res.failed = !res.accepting && res.sink;
        return res;
    }

    vector< SegrulesState > SegrulesFSA::createInitialTransitionsVector() {
        vector< SegrulesState > res(256, SegrulesState());
        const unsigned char* currPtr = ptr + initialState.offset + 1;
        const unsigned char transitionsNum = *currPtr++;
        for (int i = 0; i < transitionsNum; i++) {
            unsigned char segnum = *currPtr;
            res[segnum] = this->transition2State(currPtr);
            currPtr += 4;
        }
        return res;
    }

}
