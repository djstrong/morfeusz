/* 
 * File:   CasePatternHelper.hpp
 * Author: lennyn
 *
 * Created on April 4, 2014, 12:11 PM
 */

#ifndef CASEPATTERNHELPER_HPP
#define	CASEPATTERNHELPER_HPP

#include <vector>
#include "InterpsGroup.hpp"
#include "deserialization/morphInterps/compressionByteUtils.hpp"
#include "Environment.hpp"

namespace morfeusz {
    
class Environment;

/**
 * Utility class used to for case-sensitive interpretations filtering
 * (ie. to filter out "berlin" and keep "Berlin")
 */
class CasePatternHelper {
public:

    CasePatternHelper() : caseSensitive(false) {

    }

    /**
     * Set if this case pattern helper cares about case-sensitivity
     * 
     * @param caseSensitive
     */
    void setCaseSensitive(bool caseSensitive) {
        this->caseSensitive = caseSensitive;
    }
    
    /**
     * Check if given word matches given case pattern
     * 
     * @param lowercaseCodepoints - codepoints of checked word converter to lowercase
     * @param originalCodepoints - codepoints of checked word
     * @param casePattern - std::vector representing case pattern ( ie. [False, True] for "mBank")
     * @return - true iff word denoted by given codepoints matches given case pattern
     */
    bool checkCasePattern(
            const std::vector<uint32_t>& lowercaseCodepoints,
            const std::vector<uint32_t>& originalCodepoints,
            const std::vector<bool>& casePattern) const {
        if (this->caseSensitive) {
            for (unsigned int i = 0; i < casePattern.size(); i++) {
                if (casePattern[i] && lowercaseCodepoints[i] == originalCodepoints[i]) {
                    return false;
                }
            }
        }
        return true;
    }

    /**
     * Check if given word has a chance of matching any of case patterns in given interps group.
     * 
     * @param env - environment
     * @param orthStart - pointer to start of word
     * @param orthEnd - pointer to end of word
     * @param ig - interps group
     * @return - true iff word encoded from orthStart to orthEnd 
     *          matches at least one of the interp group's morph interpretation's case pattern.
     */
    bool checkInterpsGroupOrthCasePatterns(
            const Environment& env,
            const char* orthStart,
            const char* orthEnd,
            const InterpsGroup& ig) const;
    
    bool checkMultipleCasePatterns(
            const Environment& env,
            const char* orthStart,
            const char* orthEnd,
            const unsigned char* ptr) const;

    /**
     * Deserializes case pattern encoded at given pointer.
     * 
     * @param ptr
     * @return - case pattern
     */
    static std::vector<bool> deserializeOneCasePattern(const unsigned char*& ptr);
private:
    bool caseSensitive;
    
    mutable std::vector<uint32_t> orthCodepoints;
    mutable std::vector<uint32_t> normalizedCodepoints;

    static const uint8_t LEMMA_ONLY_LOWER = 0;
    static const uint8_t LEMMA_UPPER_PREFIX = 1;
    static const uint8_t LEMMA_MIXED_CASE = 2;
};

}

#endif	/* CASEPATTERNHELPER_HPP */

