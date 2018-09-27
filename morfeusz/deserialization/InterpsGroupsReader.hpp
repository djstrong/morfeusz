/* 
 * File:   InterpsGroupsReader.hpp
 * Author: mlenart
 *
 * Created on 16 maj 2014, 12:50
 */

#ifndef INTERPSGROUPSREADER_HPP
#define	INTERPSGROUPSREADER_HPP

#include "InterpsGroup.hpp"

namespace morfeusz {

class InterpsGroupsReader {
public:
    InterpsGroupsReader();
    InterpsGroupsReader(const unsigned char* ptr, long size);
    virtual ~InterpsGroupsReader();
    bool hasNext() const;
    InterpsGroup getNext();
    
    inline void update(const unsigned char* ptr, long size) {
        this->currPtr = ptr;
        this->endPtr = ptr + size;
    }
private:
    const unsigned char* currPtr;
    const unsigned char* endPtr;
};

}

#endif	/* INTERPSGROUPSREADER_HPP */

