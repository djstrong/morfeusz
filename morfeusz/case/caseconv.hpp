/* 
 * File:   caseconv.hpp
 * Author: mlenart
 *
 * Created on 25 listopad 2013, 16:48
 */

#ifndef CASECONV_HPP
#define	CASECONV_HPP

namespace morfeusz {

/*
 * Case conversion tables
 */

extern const unsigned int TO_LOWERCASE_TABLE_SIZE;
extern const unsigned int EXT_TO_LOWERCASE_TABLE_SIZE;
extern const uint32_t TO_LOWERCASE_TABLE[];
extern const uint32_t EXT_TO_LOWERCASE_TABLE[][2];
extern const unsigned int TO_TITLECASE_TABLE_SIZE;
extern const unsigned int EXT_TO_TITLECASE_TABLE_SIZE;
extern const uint32_t TO_TITLECASE_TABLE[];
extern const uint32_t EXT_TO_TITLECASE_TABLE[][2];

}

#endif	/* CASECONV_HPP */

