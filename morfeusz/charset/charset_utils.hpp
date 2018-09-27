/* 
 * File:   charset_utils.hpp
 * Author: lennyn
 *
 * Created on November 15, 2013, 1:57 PM
 */

#ifndef CHARSET_UTILS_HPP
#define	CHARSET_UTILS_HPP

#include <string>
#include <vector>
#include <algorithm>
#include "CharsetConverter.hpp"

namespace morfeusz {

static inline std::vector<char> initializeWhitespaces() {
    std::vector<char> res(0x3000 + 1, false);
    res[0x0000] = true; // NULL
    res[0x0009] = true; // CHARACTER TABULATION
    res[0x000A] = true; // LINE FEED (LF)
    res[0x000B] = true; // LINE TABULATION
    res[0x000C] = true; // FORM FEED (FF)
    res[0x000D] = true; // CARRIAGE RETURN (CR)
    res[0x001C] = true; // INFORMATION SEPARATOR FOUR
    res[0x001D] = true; // INFORMATION SEPARATOR THREE
    res[0x001E] = true; // INFORMATION SEPARATOR TWO
    res[0x001F] = true; // INFORMATION SEPARATOR ONE
    res[0x0020] = true; // SPACE
    res[0x0085] = true; // NEXT LINE (NEL)
    res[0x00A0] = true; // NON-BREAKING SPACE
    res[0x1680] = true; // OGHAM SPACE MARK
    res[0x180E] = true; // MONGOLIAN VOWEL SEPARATOR
    res[0x2000] = true; // EN QUAD
    res[0x2001] = true; // EM QUAD
    res[0x2002] = true; // EN SPACE
    res[0x2003] = true; // EM SPACE
    res[0x2004] = true; // THREE-PER-EM SPACE
    res[0x2005] = true; // FOUR-PER-EM SPACE
    res[0x2006] = true; // SIX-PER-EM SPACE
    res[0x2007] = true; // FIGURE SPACE
    res[0x2008] = true; // PUNCTUATION SPACE
    res[0x2009] = true; // THIN SPACE
    res[0x200A] = true; // HAIR SPACE
    res[0x2028] = true; // LINE SEPARATOR
    res[0x2029] = true; // PARAGRAPH SEPARATOR
    res[0x205F] = true; // MEDIUM MATHEMATICAL SPACE
    res[0x3000] = true; // IDEOGRAPHIC SPACE
    return res;
}

inline bool isWhitespace(uint32_t codepoint) {
    static std::vector<char> whitespaces(initializeWhitespaces());
    return codepoint < whitespaces.size() && whitespaces[codepoint];
}

}

#endif	/* CHARSET_UTILS_HPP */

