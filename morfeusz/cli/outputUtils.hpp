/* 
 * File:   outputUtils.hpp
 * Author: mlenart
 *
 * Created on 15 maj 2014, 17:45
 */

#ifndef OUTPUTUTILS_HPP
#define	OUTPUTUTILS_HPP

#include <cstdio>
#include "morfeusz2.h"

namespace morfeusz {

void printMorphResults(const Morfeusz& morfeusz, const std::vector<MorphInterpretation>& res, bool printNodeNumbers) {
    printf("[");
    int prevStart = -1;
    int prevEnd = -1;
    for (unsigned int i = 0; i < res.size(); i++) {
        const MorphInterpretation& mi = res[i];
        if (prevStart != -1
                && (prevStart != mi.startNode || prevEnd != mi.endNode)) {
            printf("]\n[");
        }
        else if (prevStart != -1) {
            printf("\n ");
        }
        //            printf("%s", mi.toString(true).c_str());
        if (printNodeNumbers) {
            printf("%d,%d,", mi.startNode, mi.endNode);
        }
        printf("%s,%s,%s,%s,%s",
                mi.orth.c_str(),
                mi.lemma.c_str(),
                mi.getTag(morfeusz).c_str(),
                mi.nameId == 0 ? "_" : mi.getName(morfeusz).c_str(),
                mi.labelsId == 0 ? "_" : mi.getLabelsAsString(morfeusz).c_str());
        prevStart = mi.startNode;
        prevEnd = mi.endNode;
    }
    printf("]\n");
}

}

#endif	/* OUTPUTUTILS_HPP */

