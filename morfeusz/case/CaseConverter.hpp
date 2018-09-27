/* 
 * File:   CaseConverter.hpp
 * Author: lennyn
 *
 * Created on November 22, 2013, 2:36 PM
 */

#ifndef CASECONVERTER_HPP
#define	CASECONVERTER_HPP

#include <inttypes.h>
#include <map>

namespace morfeusz {

class CaseConverter {
public:
    CaseConverter();
    uint32_t toLower(uint32_t codepoint) const;
    uint32_t toTitle(uint32_t codepoint) const;
private:
    std::map<uint32_t, uint32_t> extToLowercaseMap;
    std::map<uint32_t, uint32_t> extToTitlecaseMap;
};

}

#endif	/* CASECONVERTER_HPP */

