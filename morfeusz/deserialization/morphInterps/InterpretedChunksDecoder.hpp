/* 
 * File:   InterpsGroupDecoder.hpp
 * Author: mlenart
 *
 * Created on November 22, 2013, 10:35 PM
 */

#ifndef INTERPSGROUPDECODER_HPP
#define	INTERPSGROUPDECODER_HPP

#include <string>
#include <vector>
#include <utility>

#include "charset/CharsetConverter.hpp"
#include "EncodedInterpretation.hpp"
#include "InterpretedChunk.hpp"
#include "EncodedInterpretation.hpp"
#include "case/CaseConverter.hpp"
#include "Environment.hpp"
#include "morfeusz2.h"
#include "case/CasePatternHelper.hpp"
#include "compressionByteUtils.hpp"
#include "const.hpp"

namespace morfeusz {

class InterpretedChunksDecoder {
public:

    InterpretedChunksDecoder(const Environment& env) : env(env) {
    }

    virtual ~InterpretedChunksDecoder() {
    }

    virtual void decode(
            unsigned int startNode,
            unsigned int endNode,
            const InterpretedChunk& interpretedChunk,
            std::vector<MorphInterpretation>& out) const = 0;

protected:

    const unsigned char* getInterpretationsPtr(const unsigned char* igPtr) const;

    const Environment& env;
};

}

#endif	/* INTERPSGROUPDECODER_HPP */

