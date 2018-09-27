/* 
 * File:   InterpretedChunk.hpp
 * Author: mlenart
 *
 * Created on 18 listopad 2013, 15:00
 */

#ifndef INTERPRETEDCHUNK_HPP
#define	INTERPRETEDCHUNK_HPP

#include <vector>
#include "InterpsGroup.hpp"

namespace morfeusz {

/**
 * Denotes a part of text that has some not-yet-deserialized interpretations attached to it.
 */
struct InterpretedChunk {
    
    /**
     * The type of segment for this chunk.
     */
    unsigned char segmentType;
    
    /**
     * Pointer to start of word containing this chunk (possibly including prefixes text)
     */
    const char* textStartPtr;
    
    /**
     * Pointer to start of this chunks text
     */
    const char* textNoPrefixesStartPtr;
    
    /**
     * Pointer to end of this chunks text (exclusive)
     */
    const char* textEndPtr;
    
    /**
     * Pointer to start of this chunks text (exclusive)
     * possibly including preceding whitespaces if whitespace-handling set to APPEND
     */
    const char* chunkStartPtr;
    
    /**
     * Pointer to end of this chunks text (exclusive)
     * including following whitespaces if whitespace-handling set to APPEND
     */
    const char* chunkEndPtr;
    
    /**
     * Pointer to the start of this chunks binary data.
     */
    const unsigned char* interpsGroupPtr;
    
    /**
     * Pointer to the end of this chunks binary data (exclusive)
     */
    const unsigned char* interpsEndPtr;
    
    /**
     * true iff this chunk shifts orth to the one right to it (it is "A" in "A> B")
     */
    bool shiftOrth;
    
    /**
     * true iff this chunk has attached data from its prefix chunk (when it is "B" segment in "A> B" segmentation rule)
     */
    bool orthWasShifted;
    
    /**
     * Number of codepoints this chunks consists of.
     */
    int codepointsNum;
    
    /**
     * Chunks that are in the prefix segments (those with ">" in segmentation rules, ie. "dig>* dig")
     */
    std::vector<InterpretedChunk> prefixChunks;
    
    /**
     * Homonym id specified by the user.
     */
    std::string requiredHomonymId;
    
    bool forceIgnoreCase;
};

}

#endif	/* INTERPRETEDCHUNK_HPP */

