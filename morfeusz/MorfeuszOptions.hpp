/* 
 * File:   MorfeuszOptions.hpp
 * Author: mlenart
 *
 * Created on December 7, 2013, 9:36 PM
 */

#ifndef MORFEUSZOPTIONS_HPP
#define	MORFEUSZOPTIONS_HPP

#include "morfeusz2.h"

namespace morfeusz {

/**
 * Represents options for Morfeusz analyzer/generator.
 */
struct MorfeuszOptions {
    CaseHandling caseHandling;
    Charset encoding;
    TokenNumbering tokenNumbering;
    WhitespaceHandling whitespaceHandling;
    bool debug;
};

}

#endif	/* MORFEUSZOPTIONS_HPP */

