/*
 * File:   utf8sqlite.hpp
 * Author: mlenart
 *
 * Created on 24 czerwiec 2014, 15:19
 */

#ifndef UTF8SQLITE_HPP
#define	UTF8SQLITE_HPP

#include <inttypes.h>
#include <iostream>

namespace morfeusz {
    // UTF-8

    /* funkcja do pobierania kolejnych znaków w UTF-8 pozyczona od SQLite'a */

    /*
     ** This table maps from the first byte of a UTF-8 character to the number
     ** of trailing bytes expected. A value '255' indicates that the table key
     ** is not a legal first byte for a UTF-8 character.
     */
    static const unsigned char xtra_utf8_bytes[256] = {
        /* 0xxxxxxx */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

        /* 10wwwwww */
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,

        /* 110yyyyy */
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

        /* 1110zzzz */
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,

        /* 11110yyy */
        3, 3, 3, 3, 3, 3, 3, 3, 255, 255, 255, 255, 255, 255, 255, 255,
    };

    /*
     ** This table maps from the number of trailing bytes in a UTF-8 character
     ** to an integer constant that is effectively calculated for each character
     ** read by a naive implementation of a UTF-8 character reader. The code
     ** in the READ_UTF8 macro explains things best.
     */
    static const int xtra_utf8_bits[4] = {
        0,
        12416, /* (0xC0 << 6) + (0x80) */
        925824, /* (0xE0 << 12) + (0x80 << 6) + (0x80) */
        63447168 /* (0xF0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80 */
    };

#define READ_UTF8(zIn, c) { \
  int xtra;                                            \
  c = *(zIn)++;                                        \
  xtra = xtra_utf8_bytes[c];                           \
  switch( xtra ){                                      \
    case 255: c = (int)0xFFFD; break;                  \
    case 3: c = (c<<6) + *(zIn)++;                     \
    case 2: c = (c<<6) + *(zIn)++;                     \
    case 1: c = (c<<6) + *(zIn)++;                     \
    c -= xtra_utf8_bits[xtra];                         \
  }                                                    \
}

    inline uint32_t readUTF8(const unsigned char*& zIn, const unsigned char* zInEnd) {
        int xtra;
        uint32_t cp = *(zIn)++;
        xtra = xtra_utf8_bytes[cp];
        switch (xtra) {
            case 255:
                cp = 0xFFFD;
                break;
            case 3:
                cp = (zIn != zInEnd)
                        ? ((cp << 6) + *(zIn)++)
                        : 0xFFFD;
            case 2:
                cp = (zIn != zInEnd)
                        ? ((cp << 6) + *(zIn)++)
                        : 0xFFFD;
            case 1:
                cp = (zIn != zInEnd)
                        ? ((cp << 6) + *(zIn)++)
                        : 0xFFFD;
                if (cp != 0xFFFD)
                    cp -= xtra_utf8_bits[xtra];
        }
        if (cp == 0xFFFD) {
            std::cerr << "replacing invalid utf8 sequence with replacement character" << std::endl;
        }
        return cp;
    }
}

    /* koniec pożyczki */

#endif	/* UTF8SQLITE_HPP */

