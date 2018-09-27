/* 
 * File:   GroupedInterpretations.hpp
 * Author: mlenart
 *
 * Created on November 16, 2013, 7:58 PM
 */

#ifndef INTERPSGROUP_HPP
#define	INTERPSGROUP_HPP

#include <stdint.h>
#include <stddef.h>

namespace morfeusz {

/**
 * A structure representing one segment
 * with pointer to its interpretations, case patterns etc.
 */
struct InterpsGroup {
    unsigned char type;
    uint16_t size;
    const unsigned char* ptr;
};

}

#endif	/* GROUPEDINTERPRETATIONS_HPP */

