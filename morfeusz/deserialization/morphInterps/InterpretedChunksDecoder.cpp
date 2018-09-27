
#include "InterpretedChunksDecoder.hpp"

namespace morfeusz {

const unsigned char* InterpretedChunksDecoder::getInterpretationsPtr(const unsigned char* igPtr) const {
    if (env.getProcessorType() == ANALYZER) {
        if (hasCompressedOrthCasePatterns(*igPtr)) {
            return igPtr + 1;
        }
        else {
            const unsigned char* currPtr = igPtr + 1;
            unsigned char casePatternsNum = readInt8(currPtr);
            for (unsigned int i = 0; i < casePatternsNum; i++) {
                env.getCasePatternHelper().deserializeOneCasePattern(currPtr);
            }
            return currPtr;
        }
    }
    else {
        return igPtr;
    }
}

}
