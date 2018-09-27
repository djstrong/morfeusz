/* 
 * File:   InterpretedChunksDecoder4Generator.hpp
 * Author: mlenart
 *
 * Created on 15 maj 2014, 15:28
 */

#ifndef INTERPRETEDCHUNKSDECODER4GENERATOR_HPP
#define	INTERPRETEDCHUNKSDECODER4GENERATOR_HPP

#include "InterpretedChunksDecoder.hpp"

namespace morfeusz {

class InterpretedChunksDecoder4Generator : public InterpretedChunksDecoder {
public:

    InterpretedChunksDecoder4Generator(const Environment& env);

    void decode(
            unsigned int startNode,
            unsigned int endNode,
            const InterpretedChunk& interpretedChunk,
            std::vector<MorphInterpretation>& out) const;

private:

//    void convertPrefixes(const InterpretedChunk& interpretedChunk, std::string& orthPrefix, std::string& lemma) const;

    MorphInterpretation decodeMorphInterpretation(
            unsigned int startNode, unsigned int endNode,
            const std::string& orthPrefix,
            const std::string& lemma,
            const InterpretedChunk& chunk,
            const unsigned char*& ptr) const;

    void decodeForm(
            const std::vector<uint32_t>& lemma,
            const EncodedForm& orth,
            std::string& res) const;

    EncodedInterpretation deserializeInterp(const unsigned char*& ptr) const;
    
    mutable std::vector<uint32_t> codepoints;
};

}


#endif	/* INTERPRETEDCHUNKSDECODER4GENERATOR_HPP */

