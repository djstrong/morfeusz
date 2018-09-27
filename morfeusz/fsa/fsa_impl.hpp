/* 
 * File:   _simple_fsa_impl.hpp
 * Author: mlenart
 *
 * Created on October 20, 2013, 12:25 PM
 */

#ifndef _SIMPLE_FSA_IMPL_HPP
#define	_SIMPLE_FSA_IMPL_HPP

#include <cstring>
#include <algorithm>
#include <utility>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "const.hpp"
#include "utils.hpp"
#include "deserialization/endianness.hpp"

namespace morfeusz {

template <class T>
bool FSA<T>::tryToRecognize(const char* input, T& value) const {
    State<T> currState = this->getInitialState();
    int i = 0;
    while (!currState.isSink() && input[i] != '\0') {
#ifdef DEBUG_BUILD
        cerr << "proceed to next " << input[i] << endl;
#endif
        currState.proceedToNext(this, input[i]);
        i++;
    }
    // input[i] == '\0'
    //    currState.proceedToNext(0);

    if (currState.isAccepting()) {
        value = currState.getValue();
//        DEBUG(string("recognized: ")+input);
        return true;
    } else {
        return false;
    }
}

template <class T>
FSA<T>::FSA(const unsigned char* initialStatePtr, const Deserializer<T>& deserializer)
    : initialStatePtr(initialStatePtr), deserializer(deserializer) {
    
}

template <class T>
State<T> FSA<T>::getInitialState() const {
    State<T> state;
    return state;
}

template <class T>
FSA<T>* FSA<T>::getFSA(const std::string& filename, const Deserializer<T>& deserializer) {
    return getFSA(readFile<unsigned char>(filename.c_str()), deserializer);
}

template <class T>
FSA<T>* FSA<T>::getFSA(const unsigned char* ptr, const Deserializer<T>& deserializer) {
    
    uint32_t magicNumber = ntohl(*((const uint32_t*) ptr));
    if (magicNumber != MAGIC_NUMBER) {
        throw FileFormatException("Invalid file format");
    }
    
    uint8_t versionNum = *(ptr + VERSION_NUM_OFFSET);
    if (versionNum != VERSION_NUM) {
        std::ostringstream oss;
        oss << "Invalid file format version number: " << (int) versionNum << ", should be: " << (int) VERSION_NUM;
        throw FileFormatException(oss.str());
    }
    
    uint8_t implementationNum = *(ptr + IMPLEMENTATION_NUM_OFFSET);
    
    const unsigned char* startPtr = ptr + FSA_DATA_OFFSET;
    switch (implementationNum) {
        case 0:
            return new SimpleFSA<T>(startPtr, deserializer);
        case 1:
            return new CompressedFSA1<T>(startPtr, deserializer);
        case 2:
            return new CompressedFSA2<T>(startPtr, deserializer);
        default:
            std::ostringstream oss;
            oss << "Invalid implementation number: " << versionNum << ", should be: " << VERSION_NUM;
            throw FileFormatException(oss.str());
    }
}

}

#endif	/* _SIMPLE_FSA_IMPL_HPP */
