/* 
 * File:   MorphDeserializer.hpp
 * Author: mlenart
 *
 * Created on 16 maj 2014, 12:46
 */

#ifndef MORPHDESERIALIZER_HPP
#define	MORPHDESERIALIZER_HPP

#include "fsa/fsa.hpp"
#include "InterpsGroup.hpp"
#include "InterpsGroupsReader.hpp"

namespace morfeusz {

class MorphDeserializer: public Deserializer< InterpsGroupsReader > {
public:
    MorphDeserializer();
    virtual ~MorphDeserializer();
    long deserialize(
        const unsigned char* ptr, 
        InterpsGroupsReader& res) const;
private:

};

}

#endif	/* MORPHDESERIALIZER_HPP */

