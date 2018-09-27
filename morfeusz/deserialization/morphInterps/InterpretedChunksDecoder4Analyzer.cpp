/* 
 * File:   InterpretedChunksDecoder4Analyzer.cpp
 * Author: mlenart
 * 
 * Created on 15 maj 2014, 15:28
 */

#include "InterpretedChunksDecoder4Analyzer.hpp"
#include <string>

using namespace std;

namespace morfeusz {

    InterpretedChunksDecoder4Analyzer::InterpretedChunksDecoder4Analyzer(const Environment& env) : InterpretedChunksDecoder(env) {
    }

    void InterpretedChunksDecoder4Analyzer::decode(
            unsigned int startNode,
            unsigned int endNode,
            const InterpretedChunk& interpretedChunk,
            std::vector<MorphInterpretation>& out) const {
        string orth;
        string lemma4Prefixes;
        if (tryToGetLemma4Prefixes(interpretedChunk, lemma4Prefixes)) {
            orth.insert(orth.end(), interpretedChunk.chunkStartPtr, interpretedChunk.chunkEndPtr);
            const unsigned char* currPtr = getInterpretationsPtr(interpretedChunk.interpsGroupPtr);
            while (currPtr < interpretedChunk.interpsEndPtr) {
                DecodeMorphInterpParams params = {startNode, endNode, orth, lemma4Prefixes, interpretedChunk};
                this->decodeMorphInterpretation(params, currPtr, out);
            }
        }
    }

    void InterpretedChunksDecoder4Analyzer::decodeLemma(
            const EncodedForm& encodedLemma,
            int nonPrefixCodepointsNum,
            bool forPrefix,
            string& res) const {
        unsigned int prefixSegmentsOrthLength = forPrefix
                ? 0
                : (unsigned int) normalizedCodepoints.size() - nonPrefixCodepointsNum;
        size_t endIdx = forPrefix
                ? normalizedCodepoints.size()
                : normalizedCodepoints.size() - encodedLemma.suffixToCut;
        for (unsigned int i = prefixSegmentsOrthLength + encodedLemma.prefixToCut; i < endIdx; i++) {
            uint32_t cp =
                    (i < encodedLemma.casePattern.size() && encodedLemma.casePattern[i])
                    ? env.getCaseConverter().toTitle(normalizedCodepoints[i])
                    : normalizedCodepoints[i];
            env.getCharsetConverter().append(cp, res);
        }
        if (!forPrefix) {
            const char* suffixPtr = encodedLemma.suffixToAdd.c_str();
            const char* suffixEnd = suffixPtr + encodedLemma.suffixToAdd.length();
            while (suffixPtr != suffixEnd) {
                uint32_t cp = UTF8CharsetConverter::getInstance().next(suffixPtr, suffixEnd);
                env.getCharsetConverter().append(cp, res);
            }
        }
    }

    void InterpretedChunksDecoder4Analyzer::decodeEncodedForm(const unsigned char*& ptr, unsigned char compressionByte, EncodedForm& encodedForm) const {
        encodedForm.prefixToCut = hasCompressedPrefixCut(compressionByte)
                ? getPrefixCutLength(compressionByte)
                : readInt8(ptr);
        encodedForm.suffixToCut = readInt8(ptr);
        encodedForm.suffixToAdd = readString(ptr);
        assert(encodedForm.casePattern.size() == 0);
        if (isLemmaOnlyLower(compressionByte)) {
            // do nothing - empty case pattern
        } else if (isLemmaOnlyTitle(compressionByte)) {
            encodedForm.casePattern.push_back(true);
        } else {
            encodedForm.casePattern = env.getCasePatternHelper().deserializeOneCasePattern(ptr);
        }
    }

    EncodedInterpretation InterpretedChunksDecoder4Analyzer::decodeEncodedInterp(const unsigned char*& ptr, unsigned char compressionByte) const {
        EncodedInterpretation interp;
        if (isOrthOnlyLower(compressionByte)) {
        } else if (isOrthOnlyTitle(compressionByte)) {
            interp.orthCasePattern.push_back(true);
        } else {
            interp.orthCasePattern = this->env.getCasePatternHelper().deserializeOneCasePattern(ptr);
        }
        decodeEncodedForm(ptr, compressionByte, interp.value);
        interp.tag = readInt16(ptr);
        interp.nameClassifier = *ptr++;
        interp.qualifiers = readInt16(ptr);
        return interp;
    }
    
    // this function is to make "niePolak" recognized but not "Niepolak" (for segmentation: "nie> Polak")
    static void addPrefixLengthToCasePatterns(const InterpretedChunk& chunk, EncodedInterpretation& ei) {
        for (unsigned int i = 0; i < chunk.prefixChunks.size(); i++) {
            const InterpretedChunk& prefixChunk = chunk.prefixChunks[i];
            ei.orthCasePattern.insert(ei.orthCasePattern.begin(), prefixChunk.codepointsNum, false);
            ei.value.casePattern.insert(ei.value.casePattern.begin(), prefixChunk.codepointsNum, false);
        }
    }

    void InterpretedChunksDecoder4Analyzer::decodeMorphInterpretation(
            const DecodeMorphInterpParams& params,
            const unsigned char*& ptr,
            std::vector<MorphInterpretation>& out) const {
        orthCodepoints.resize(0);
        normalizedCodepoints.resize(0);
        const char* currPtr = params.chunk.textStartPtr;
        while (currPtr != params.chunk.textEndPtr) {
            uint32_t cp = env.getCharsetConverter().next(currPtr, params.chunk.textEndPtr);
            orthCodepoints.push_back(cp);
            normalizedCodepoints.push_back(env.getCaseConverter().toLower(cp));
        }
        EncodedInterpretation ei = this->decodeEncodedInterp(ptr, *params.chunk.interpsGroupPtr);
        
        if (!ei.orthCasePattern.empty() && !params.chunk.prefixChunks.empty()) {
            addPrefixLengthToCasePatterns(params.chunk, ei);
        }
        
        if (params.chunk.forceIgnoreCase || env.getCasePatternHelper().checkCasePattern(normalizedCodepoints, orthCodepoints, ei.orthCasePattern)) {
            string lemma(params.lemma4Prefixes);
            lemma.reserve(lemma.size() + normalizedCodepoints.size());
            this->decodeLemma(ei.value, params.chunk.codepointsNum, false, lemma);
            size_t newIdx = out.size();
            out.resize(newIdx + 1);
            MorphInterpretation& newElem = out[newIdx];
            newElem.startNode = params.startNode;
            newElem.endNode = params.endNode;
            newElem.orth = params.orth;
            newElem.lemma = lemma;
            newElem.tagId = ei.tag;
            newElem.nameId = ei.nameClassifier;
            newElem.labelsId = ei.qualifiers;
        }
    }

    bool InterpretedChunksDecoder4Analyzer::tryToGetLemma4Prefixes(
            const InterpretedChunk& interpretedChunk,
            string& lemma4Prefixes) const {
        for (unsigned int i = 0; i < interpretedChunk.prefixChunks.size(); i++) {
            const InterpretedChunk& prefixChunk = interpretedChunk.prefixChunks[i];
            if (!tryToGetLemma4OnePrefix(prefixChunk, lemma4Prefixes)) {
                return false;
            }
        }
        return true;
    }

    bool InterpretedChunksDecoder4Analyzer::tryToGetLemma4OnePrefix(const InterpretedChunk& prefixChunk, std::string& lemma4Prefixes) const {
        orthCodepoints.resize(0);
        normalizedCodepoints.resize(0);
        const char* currTextPtr = prefixChunk.textNoPrefixesStartPtr;
        while (currTextPtr != prefixChunk.textEndPtr) {
            uint32_t cp = env.getCharsetConverter().next(currTextPtr, prefixChunk.textEndPtr);
            orthCodepoints.push_back(cp);
            normalizedCodepoints.push_back(env.getCaseConverter().toLower(cp));
        }
        const unsigned char* currPtr = getInterpretationsPtr(prefixChunk.interpsGroupPtr);
        EncodedInterpretation ei = this->decodeEncodedInterp(currPtr, *prefixChunk.interpsGroupPtr);
        if (prefixChunk.forceIgnoreCase || env.getCasePatternHelper().checkCasePattern(normalizedCodepoints, orthCodepoints, ei.orthCasePattern)) {
            this->decodeLemma(ei.value, prefixChunk.codepointsNum, true, lemma4Prefixes);
            return true;
        } else {
            return false;
        }
    }

}
