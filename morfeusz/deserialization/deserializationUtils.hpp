/* 
 * File:   deserializationUtils.hpp
 * Author: lennyn
 *
 * Created on April 4, 2014, 6:07 PM
 */

#ifndef DESERIALIZATIONUTILS_HPP
#define	DESERIALIZATIONUTILS_HPP

#include "endianness.hpp"
#include "fsa/const.hpp"
#include <iostream>
#include <vector>

namespace morfeusz {

inline unsigned char readInt8(const unsigned char*& currPtr) {
    return *currPtr++;
}

inline uint16_t readInt16(const unsigned char*& currPtr) {
    uint16_t res = htons(*reinterpret_cast<const uint16_t*> (currPtr));
    currPtr += 2;
    return res;
}

inline unsigned int readInt24(const unsigned char*& currPtr) {
    unsigned int res = currPtr[0] << 16 | currPtr[1] << 8 | currPtr[2];
    currPtr += 3;
    return res;
}

inline uint32_t readInt32(const unsigned char*& currPtr) {
    uint32_t res = htonl(*reinterpret_cast<const uint32_t*> (currPtr));
    currPtr += 4;
    return res;
}

inline uint32_t readInt32Const(const unsigned char* const currPtr) {
    uint32_t res = htonl(*reinterpret_cast<const uint32_t*> (currPtr));
    return res;
}

inline std::string readString(const unsigned char*& currPtr) {
    std::string res((const char*) currPtr);
    currPtr += res.length();
    currPtr++;
    return res;
}

inline const unsigned char* getEpiloguePtr(const unsigned char* ptr) {
    uint32_t fsaSize = readInt32Const(ptr + FSA_DATA_SIZE_OFFSET);
    return ptr + FSA_DATA_OFFSET + fsaSize;
}
}

#endif	/* DESERIALIZATIONUTILS_HPP */

