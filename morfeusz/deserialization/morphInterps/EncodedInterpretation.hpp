/* 
 * File:   interpretation.hpp
 * Author: mlenart
 *
 * Created on November 4, 2013, 3:11 PM
 */

#ifndef INTERPRETATION_HPP
#define	INTERPRETATION_HPP

#include <string>
#include <vector>
#include <inttypes.h>
#include "IdResolverImpl.hpp"

namespace morfeusz {

/*
 * Lemma in a compressed format (as in an automaton)
 */
struct EncodedForm {
    unsigned char prefixToCut;
    unsigned char suffixToCut;
    std::string suffixToAdd;
    std::vector<bool> casePattern;
    std::string prefixToAdd;
};

/*
 * Internal representation of an interpretation - with lemma encoded
 */
struct EncodedInterpretation {
    std::vector<bool> orthCasePattern;
    EncodedForm value;
    int tag;
    int nameClassifier;
    std::string homonymId;
    int qualifiers;
};

}

#endif	/* INTERPRETATION_HPP */
