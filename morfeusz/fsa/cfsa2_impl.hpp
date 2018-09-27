/* 
 * File:   _vfsa_impl.hpp
 * Author: lennyn
 *
 * Created on October 29, 2013, 9:57 PM
 */

#ifndef _VFSA_IMPL_HPP
#define	_VFSA_IMPL_HPP

#include <algorithm>
#include <utility>
#include <iostream>
#include "fsa.hpp"
#include "../utils.hpp"
#include "../deserialization/endianness.hpp"

namespace morfeusz {

static const unsigned char HAS_REMAINING_FLAG = 128;
static const unsigned char ACCEPTING_FLAG = 64;
static const unsigned char LAST_FLAG = 32;
static const unsigned char OFFSET_MASK = 0x7F; // 0b01111111;
static const unsigned char FIRST_BYTE_OFFSET_MASK = 0x1F; // 0b00011111;

template <class T>
std::vector<unsigned char> CompressedFSA2<T>::initializeChar2PopularCharIdx(const unsigned char* ptr) {
    std::vector<unsigned char> res;
    return res;
    //    return std::vector<unsigned char>(ptr + getPopularCharsOffset(), ptr + getPopularCharsOffset() + 256);
}

template <class T>
CompressedFSA2<T>::CompressedFSA2(const unsigned char* ptr, const Deserializer<T>& deserializer)
: FSA<T>(ptr, deserializer) {
}

template <class T>
CompressedFSA2<T>::~CompressedFSA2() {

}

template <class T>
void CompressedFSA2<T>::reallyDoProceed(
        const unsigned char* statePtr,
        const bool accepting,
        State<T>& state) const {
    if (accepting) {
        T object;
        long size = this->deserializer.deserialize(statePtr, object);
        state.setNext(statePtr - this->initialStatePtr, object, size);
    } else {
        state.setNext(statePtr - this->initialStatePtr);
    }
}

static inline void passThroughOffsetBytes(unsigned char*& ptr) {
    while (*ptr & HAS_REMAINING_FLAG) {
        ptr++;
    }
    ptr++;
}

static inline unsigned int readOffsetBytes(unsigned char*& ptr) {
    unsigned int res = *ptr & FIRST_BYTE_OFFSET_MASK;
    if (*ptr & HAS_REMAINING_FLAG) {
        ptr++;
        while (*ptr & HAS_REMAINING_FLAG) {
            res <<= 7;
            res += *ptr & OFFSET_MASK;
            ptr++;
        }
        res <<= 7;
        res += *ptr & OFFSET_MASK;
    }
    ptr++;
    return res;
}

template <class T>
void CompressedFSA2<T>::doProceedToNextByList(
        const char c,
        const unsigned char* ptr,
        State<T>& state) const {
    unsigned char* currPtr = const_cast<unsigned char*> (ptr);
    while (true) {
        //        const_cast<Counter*>(&counter)->increment(1);
        if ((char) *currPtr == c) {
            currPtr++;
            bool accepting = *currPtr & ACCEPTING_FLAG;
            unsigned int offset = readOffsetBytes(currPtr);
            currPtr += offset;
            reallyDoProceed(currPtr, accepting, state);
            
            return;
        } 
        else {
            currPtr++;
            if (*currPtr & LAST_FLAG) {
                state.setNextAsSink();
                return;
            }
            else {
                passThroughOffsetBytes(currPtr);
            }
        }
    }
}

template <class T>
void CompressedFSA2<T>::proceedToNext(const char c, State<T>& state) const {
#ifdef DEBUG_BUILD
    if (c <= 'z' && 'a' <= c)
        cerr << "NEXT " << c << " from " << state.getOffset() << endl;
    else
        cerr << "NEXT " << (short) c << " from " << state.getOffset() << endl;
#endif
    const unsigned char* fromPointer = this->initialStatePtr + state.getOffset();
    unsigned long transitionsTableOffset = 0;
    if (state.isAccepting()) {
        transitionsTableOffset += state.getValueSize();
    }
    this->doProceedToNextByList(
            c,
            fromPointer + transitionsTableOffset,
            state);
}

}

#endif	/* _VFSA_IMPL_HPP */

