/* 
 * File:   const.hpp
 * Author: lennyn
 *
 * Created on December 3, 2013, 9:04 PM
 */

#ifndef CONST1_HPP
#define	CONST1_HPP

#include "morfeusz2.h"
#include <string>

namespace morfeusz {

enum MorfeuszProcessorType {
    GENERATOR = 101,
    ANALYZER = 102
};

extern const Charset DEFAULT_MORFEUSZ_CHARSET;

extern const unsigned char SHIFT_ORTH_NODE;

extern const char HOMONYM_SEPARATOR;

extern const char LABELS_SEPARATOR;

extern const char FILESYSTEM_PATH_SEPARATOR;

extern const std::string COPYRIGHT_TEXT;

}

#endif	/* CONST_HPP */
