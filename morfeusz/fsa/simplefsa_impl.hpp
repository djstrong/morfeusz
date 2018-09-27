/* 
 * File:   simplefsa_impl.hpp
 * Author: mlenart
 *
 * Created on November 4, 2013, 1:05 PM
 */

#ifndef SIMPLEFSA_IMPL_HPP
#define	SIMPLEFSA_IMPL_HPP

#include <iostream>

#include "fsa.hpp"

//#pragma pack(push, 1)  /* push current alignment to stack */

namespace morfeusz {

struct StateData {
    unsigned char transitionsNum;
    bool accepting;
};

//#pragma pack(pop)   /* restore original alignment from stack */

template <class T>
SimpleFSA<T>::SimpleFSA(const unsigned char* ptr, const Deserializer<T>& deserializer, bool isTransducer)
: FSA<T>(ptr, deserializer), isTransducer(isTransducer) {
}

template <class T>
SimpleFSA<T>::~SimpleFSA() {

}

static inline unsigned int decodeOffset(const unsigned char* ptr) {
    unsigned int res = 0;
    res = ptr[0] << 16 | ptr[1] << 8 | ptr[2];
    return res;
}

static inline StateData decodeStateData(const unsigned char* ptr) {
    static const unsigned char acceptingFlag = 128;
    static const unsigned char transitionsNumMask = 127;
    StateData res;
    res.transitionsNum = (*ptr) & transitionsNumMask;
    res.accepting = (*ptr) & acceptingFlag;
    return res;
}

template <class T>
void SimpleFSA<T>::proceedToNext(const char c, State<T>& state) const {
    const unsigned char* fromPointer = this->initialStatePtr + state.getOffset();
    long transitionsTableOffset = 1;
    if (state.isAccepting()) {
        transitionsTableOffset += state.getValueSize();
    }
    StateData stateData = decodeStateData(fromPointer);
    const unsigned char* foundTransition = fromPointer + transitionsTableOffset;
    bool found = false;
    unsigned int increment = this->isTransducer ? 5 : 4;
    for (unsigned int i = 0; i < stateData.transitionsNum; i++, foundTransition += increment) {
        if ((char) *foundTransition == c) {
            found = true;
            break;
        }
    }
    if (!found) {
        state.setNextAsSink();
    }
    else {
        unsigned int offset = decodeOffset(foundTransition + 1);
        const unsigned char* nextStatePointer = this->initialStatePtr + offset;
        StateData nextStateData = decodeStateData(nextStatePointer);
        if (nextStateData.accepting) {
            T object;
            long size = this->deserializer.deserialize(nextStatePointer + 1, object);
            state.setNext(offset, object, size);
        }
        else {
            state.setNext(offset);
        }
    }
}

}

#endif	/* SIMPLEFSA_IMPL_HPP */

