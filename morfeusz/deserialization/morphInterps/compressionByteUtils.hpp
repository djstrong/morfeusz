/* 
 * File:   compressionByteUtils.hpp
 * Author: lennyn
 *
 * Created on April 23, 2014, 4:38 PM
 */

#ifndef COMPRESSIONBYTEUTILS_HPP
#define	COMPRESSIONBYTEUTILS_HPP

#include "InterpsGroup.hpp"

namespace morfeusz {

class CompressionByteFlags {
public:
    static const unsigned char ORTH_ONLY_LOWER = 128;
    static const unsigned char ORTH_ONLY_TITLE = 64;
    static const unsigned char LEMMA_ONLY_LOWER = 32;
    static const unsigned char LEMMA_ONLY_TITLE = 16;
    static const unsigned char PREFIX_CUT_MASK = 15;
};

inline bool hasCompressedOrthCasePatterns(unsigned char byte) {
    return byte & (CompressionByteFlags::ORTH_ONLY_LOWER | CompressionByteFlags::ORTH_ONLY_TITLE);
}

inline bool isOrthOnlyLower(unsigned char byte) {
    return byte & CompressionByteFlags::ORTH_ONLY_LOWER;
}

inline bool isOrthOnlyTitle(unsigned char byte) {
    return byte & CompressionByteFlags::ORTH_ONLY_TITLE;
}

inline bool isLemmaOnlyLower(unsigned char byte) {
    return byte & CompressionByteFlags::LEMMA_ONLY_LOWER;
}

inline bool isLemmaOnlyTitle(unsigned char byte) {
    return byte & CompressionByteFlags::LEMMA_ONLY_TITLE;
}

inline bool hasCompressedPrefixCut(unsigned char byte) {
    return (byte & CompressionByteFlags::PREFIX_CUT_MASK) != CompressionByteFlags::PREFIX_CUT_MASK;
}

inline unsigned char getPrefixCutLength(unsigned char byte) {
    return byte & CompressionByteFlags::PREFIX_CUT_MASK;
}

}

#endif	/* COMPRESSIONBYTEUTILS_HPP */

