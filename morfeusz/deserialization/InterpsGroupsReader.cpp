/* 
 * File:   InterpsGroupsReader.cpp
 * Author: mlenart
 * 
 * Created on 16 maj 2014, 12:50
 */

#include "InterpsGroupsReader.hpp"
#include "deserialization/deserializationUtils.hpp"

namespace morfeusz {

InterpsGroupsReader::InterpsGroupsReader()
: currPtr(NULL), endPtr(NULL) {
    
}

InterpsGroupsReader::InterpsGroupsReader(const unsigned char* ptr, long size)
: currPtr(ptr), endPtr(ptr + size) {
    
}

InterpsGroupsReader::~InterpsGroupsReader() {
    
}

bool InterpsGroupsReader::hasNext() const {
    return currPtr < endPtr;
}

InterpsGroup InterpsGroupsReader::getNext() {
    InterpsGroup ig;
    ig.type = readInt8(currPtr);
    ig.size = readInt16(currPtr);
    ig.ptr = currPtr;
    currPtr += ig.size;
    return ig;
}

}
