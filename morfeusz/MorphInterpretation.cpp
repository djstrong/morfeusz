/* 
 * File:   MorphInterpretation.cpp
 * Author: mlenart
 * 
 * Created on November 14, 2013, 11:47 AM
 */

#include <string>
#include <sstream>
#include "morfeusz2.h"
#include "Environment.hpp"
#include "const.hpp"

using namespace std;

namespace morfeusz {

    MorphInterpretation MorphInterpretation::createIgn(
            int startNode, int endNode,
            const std::string& orth,
            const std::string& lemma) {
        MorphInterpretation mi;
        mi.startNode = startNode;
        mi.endNode = endNode;
        mi.orth = orth;
        mi.lemma = lemma;
        mi.tagId = 0;
        mi.nameId = 0;
        mi.labelsId = 0;
        return mi;
    }

    /**
     * Creates new instance with "sp" tag (meaning: "this is a sequence of whitespaces")
     */
    MorphInterpretation MorphInterpretation::createWhitespace(int startNode, int endNode, const std::string& orth) {
        MorphInterpretation mi;
        mi.startNode = startNode;
        mi.endNode = endNode;
        mi.orth = orth;
        mi.lemma = orth;
        mi.tagId = 1;
        mi.nameId = 0;
        mi.labelsId = 0;
        return mi;
    }

}
