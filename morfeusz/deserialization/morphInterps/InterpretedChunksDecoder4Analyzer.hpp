/* 
 * File:   InterpretedChunksDecoder4Analyzer.hpp
 * Author: mlenart
 *
 * Created on 15 maj 2014, 15:28
 */

#ifndef INTERPRETEDCHUNKSDECODER4ANALYZER_HPP
#define	INTERPRETEDCHUNKSDECODER4ANALYZER_HPP

#include "InterpretedChunksDecoder.hpp"
#include "InterpretedChunk.hpp"

namespace morfeusz {

struct DecodeMorphInterpParams {
    unsigned int startNode;
    unsigned int endNode;
    const std::string& orth;
    const std::string& lemma4Prefixes;
    const InterpretedChunk& chunk;
};

class InterpretedChunksDecoder4Analyzer : public InterpretedChunksDecoder {
public:

    InterpretedChunksDecoder4Analyzer(const Environment& env);

    void decode(
            unsigned int startNode,
            unsigned int endNode,
            const InterpretedChunk& interpretedChunk,
            std::vector<MorphInterpretation>& out) const;

private:

    void decodeLemma(
            const EncodedForm& lemma,
            int realOrthCodepointsNum,
            bool forPrefix,
            std::string& res) const;

    void decodeEncodedForm(const unsigned char*& ptr, unsigned char compressionByte, EncodedForm& encodedForm) const;

    EncodedInterpretation decodeEncodedInterp(const unsigned char*& ptr, unsigned char compressionByte) const;

    void decodeMorphInterpretation(const DecodeMorphInterpParams& params, const unsigned char*& ptr, std::vector<MorphInterpretation>& out) const;

    bool tryToGetLemma4Prefixes(const InterpretedChunk& interpretedChunk, std::string& lemma4Prefixes) const;
    
    bool tryToGetLemma4OnePrefix(const InterpretedChunk& prefixChunk, std::string& lemma4Prefixes) const;
    
    mutable std::vector<uint32_t> orthCodepoints;
    mutable std::vector<uint32_t> normalizedCodepoints;
};

}

#endif	/* INTERPRETEDCHUNKSDECODER4ANALYZER_HPP */

