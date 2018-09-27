/* 
 * File:   cfsa1_impl.hpp
 * Author: mlenart
 *
 * Created on November 4, 2013, 1:08 PM
 */

#ifndef CFSA1_IMPL_HPP
#define	CFSA1_IMPL_HPP

#include <vector>
#include <climits>

#include "fsa.hpp"
#include "../deserialization/deserializationUtils.hpp"

namespace morfeusz {

static const unsigned char CFSA1_ACCEPTING_FLAG = 128;
//static const unsigned char CFSA1_ARRAY_FLAG = 64;
static const unsigned char CFSA1_TRANSITIONS_NUM_MASK = 127;

static const unsigned char CFSA1_OFFSET_SIZE_MASK = 3;

static const unsigned int CFSA1_INITIAL_ARRAY_STATE_OFFSET = 257;

struct StateData2 {
    unsigned int transitionsNum;
    bool isAccepting;
};

struct TransitionData2 {
    unsigned int offsetSize;
    unsigned int shortLabel;
};

static inline StateData2 readStateData(const unsigned char*& ptr) {
    StateData2 res;
    unsigned char firstByte = readInt8(ptr);
    res.isAccepting = firstByte & CFSA1_ACCEPTING_FLAG;
    res.transitionsNum = firstByte & CFSA1_TRANSITIONS_NUM_MASK;
    if (res.transitionsNum == CFSA1_TRANSITIONS_NUM_MASK) {
        res.transitionsNum = readInt8(ptr);
    }
    return res;
}

static inline TransitionData2 readTransitionFirstByte(const unsigned char*& ptr) {
    TransitionData2 res;
    unsigned char firstByte = readInt8(ptr);
    res.offsetSize = firstByte & CFSA1_OFFSET_SIZE_MASK;
    res.shortLabel = firstByte >> 2;
    return res;
}

template <class T>
std::vector<unsigned char> CompressedFSA1<T>::initializeChar2PopularCharIdx(const unsigned char* ptr) {
    std::vector<unsigned char> res(ptr, ptr + CFSA1_INITIAL_ARRAY_STATE_OFFSET);
    return res;
}

template <class T>
void CompressedFSA1<T>::initializeInitialTransitions() {
    hasTransition = std::vector<char>(256, false);
    initialTransitions = std::vector< State<T> >(256);
    char c = CHAR_MIN;
    while (true) {
        State<T> state;
        doProceedToNext(c, state, true);
        int currIdx = static_cast<const unsigned char>(c);
        initialTransitions[currIdx] = state;
        if (c == CHAR_MAX) {
            return;
        }
        else {
            c++;
        }
    }
}

template <class T>
CompressedFSA1<T>::CompressedFSA1(const unsigned char* ptr, const Deserializer<T>& deserializer)
: FSA<T>(ptr + CFSA1_INITIAL_ARRAY_STATE_OFFSET, deserializer),
label2ShortLabel(initializeChar2PopularCharIdx(ptr)),
hasTransition(),
initialTransitions() {
    initializeInitialTransitions();
}

template <class T>
CompressedFSA1<T>::~CompressedFSA1() {

}

template <class T>
void CompressedFSA1<T>::reallyDoProceed(
        const unsigned char* statePtr,
        State<T>& state) const {
    const unsigned char* currPtr = statePtr;
    const StateData2 sd = readStateData(currPtr);
    if (sd.isAccepting) {
        T value;
        long valueSize = this->deserializer.deserialize(currPtr, value);
        state.setNext(statePtr - this->initialStatePtr, value, valueSize);
    }
    else {
        state.setNext(statePtr - this->initialStatePtr);
    }
}

template <class T>
void CompressedFSA1<T>::proceedToNext(const char c, State<T>& state) const {
    doProceedToNext(c, state, false);
}

template <class T>
void CompressedFSA1<T>::doProceedToNext(const char c, State<T>& state, bool initial) const {
    if (state.offset == 0 && !initial) {
        int idx = static_cast<const unsigned char>(c);
        State<T> newState = this->initialTransitions[idx];
        state = newState;
        return;
    }
    const unsigned char* currPtr = this->initialStatePtr + state.getOffset();
    unsigned char shortLabel = this->label2ShortLabel[(const unsigned char) c];
    const StateData2 sd = readStateData(currPtr);
    if (state.isAccepting()) {
        currPtr += state.getValueSize();
    }
    bool found = false;
    TransitionData2 td;
    for (unsigned int i = 0; i < sd.transitionsNum; i++) {
        td = readTransitionFirstByte(currPtr);
        if (td.shortLabel == shortLabel) {
            if (shortLabel == 0) {
                char label = static_cast<char> (readInt8(currPtr));
                if (label == c) {
                    found = true;
                    break;
                }
                else {
                    currPtr += td.offsetSize;
                }
            } 
            else {
                found = true;
                break;
            }
        }
        else {
            if (td.shortLabel == 0) {
                currPtr++;
            }
            currPtr += td.offsetSize;
        }
    }
    if (!found) {
        state.setNextAsSink();
    } 
    else {
        uint32_t offset;
        switch (td.offsetSize) {
            case 0:
                offset = 0;
                break;
            case 1:
                offset = readInt8(currPtr);
                break;
            case 2:
                offset = readInt16(currPtr);
                break;
            case 3:
                offset = readInt24(currPtr);
                break;
            default:
                std::cerr << "Offset size = " << td.offsetSize << std::endl;
                throw FileFormatException("Invalid file format");
        }
        currPtr += offset;
        reallyDoProceed(currPtr, state);
    }
}

}

#endif	/* CFSA1_IMPL_HPP */

