
#include "CasePatternHelper.hpp"

using namespace std;

namespace morfeusz {

    bool CasePatternHelper::checkInterpsGroupOrthCasePatterns(
            const Environment& env,
            const char* orthStart,
            const char* orthEnd,
            const InterpsGroup& ig) const {
        const unsigned char* currPtr = ig.ptr;
        unsigned char compressionByte = *currPtr++;
        if (!this->caseSensitive || isOrthOnlyLower(compressionByte)) {
            return true;
        }
        else if (isOrthOnlyTitle(compressionByte)) {
            uint32_t cp = env.getCharsetConverter().next(orthStart, orthEnd);
            return cp == env.getCaseConverter().toTitle(cp);
        }
        else {
            return checkMultipleCasePatterns(env, orthStart, orthEnd, currPtr);
        }
    }

    bool CasePatternHelper::checkMultipleCasePatterns(
            const Environment& env,
            const char* orthStart,
            const char* orthEnd,
            const unsigned char* ptr) const {
        unsigned char casePatternsNum = *ptr++;
        if (casePatternsNum == 0) {
            return true;
        }
        else {
            const char* currOrthPtr = orthStart;
            orthCodepoints.resize(0);
            normalizedCodepoints.resize(0);
            bool isDiff = false;
            while (currOrthPtr != orthEnd) {
                uint32_t codepoint = env.getCharsetConverter().next(currOrthPtr, orthEnd);
                uint32_t normalizedCodepoint = env.getCaseConverter().toLower(codepoint);
                orthCodepoints.push_back(codepoint);
                normalizedCodepoints.push_back(normalizedCodepoint);
                isDiff = isDiff || codepoint != normalizedCodepoint;
            }
            if (!isDiff) {
                return false;
            }
            else
                for (unsigned int i = 0; i < casePatternsNum; i++) {
                    if (isDiff && checkCasePattern(
                            normalizedCodepoints,
                            orthCodepoints,
                            deserializeOneCasePattern(ptr))) {
                        return true;
                    }
                }
            return false;
        }
    }

    std::vector<bool> CasePatternHelper::deserializeOneCasePattern(const unsigned char*& ptr) {
        std::vector<bool> res;
        uint8_t casePatternType = *ptr++;
        uint8_t prefixLength;
        uint8_t patternLength;
        switch (casePatternType) {
            case LEMMA_ONLY_LOWER:
                break;
            case LEMMA_UPPER_PREFIX:
                prefixLength = *ptr++;
                res.resize(prefixLength, true);
                break;
            case LEMMA_MIXED_CASE:
                patternLength = *ptr++;
                for (unsigned int i = 0; i < patternLength; i++) {
                    uint8_t idx = *ptr++;
                    res.resize(idx + 1, false);
                    res[idx] = true;
                }
                break;
        }
        return res;
    }

}
