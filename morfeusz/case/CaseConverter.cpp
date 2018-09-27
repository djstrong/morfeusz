/* 
 * File:   CaseConverter.cpp
 * Author: lennyn
 * 
 * Created on November 22, 2013, 2:36 PM
 */

#include "CaseConverter.hpp"
#include "caseconv.hpp"

using namespace std;

namespace morfeusz {

map<uint32_t, uint32_t> initializeExtCaseMap(const uint32_t (*table)[2], unsigned int tableSize) {
    map<uint32_t, uint32_t> res;
    for (unsigned int i = 0; i < tableSize; i++) {
        uint32_t key = table[i][0];
        uint32_t value = table[i][1];
        res[key] = value;
    }
    return res;
}

CaseConverter::CaseConverter()
: extToLowercaseMap(initializeExtCaseMap(EXT_TO_LOWERCASE_TABLE, EXT_TO_LOWERCASE_TABLE_SIZE)),
extToTitlecaseMap(initializeExtCaseMap(EXT_TO_TITLECASE_TABLE, EXT_TO_TITLECASE_TABLE_SIZE)) {
}

static uint32_t getFromTables(const uint32_t* table, unsigned int tableSize, const map<uint32_t, uint32_t>& extMap, const uint32_t codepoint) {
    if (codepoint < tableSize) {
        return table[codepoint];
    }
    else if (extMap.count(codepoint) != 0) {
        map<uint32_t, uint32_t>::const_iterator it;
        it = extMap.find(codepoint);
        return it->second;
    }
    else {
        return codepoint;
    }
}

uint32_t CaseConverter::toLower(uint32_t codepoint) const {
    return getFromTables(TO_LOWERCASE_TABLE, TO_LOWERCASE_TABLE_SIZE, this->extToLowercaseMap, codepoint);
}

uint32_t CaseConverter::toTitle(uint32_t codepoint) const {
    return getFromTables(TO_TITLECASE_TABLE, TO_TITLECASE_TABLE_SIZE, this->extToTitlecaseMap, codepoint);
}

}
