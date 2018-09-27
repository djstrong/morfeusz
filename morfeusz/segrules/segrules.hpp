/* 
 * File:   segrules.hpp
 * Author: mlenart
 *
 * Created on 25 luty 2014, 16:35
 */

#ifndef SEGRULES_HPP
#define	SEGRULES_HPP

#include <utility>
#include <map>
#include <string>
#include <vector>
#include "SegrulesFSA.hpp"

namespace morfeusz {

typedef std::map<std::string, std::string> SegrulesOptions;
//typedef State<unsigned char> SegrulesStateType;
//typedef FSA<unsigned char> SegrulesFSAType;

std::map<SegrulesOptions, SegrulesFSA*> createSegrulesFSAsMap(const unsigned char* analyzerPtr);
SegrulesOptions getDefaultSegrulesOptions(const unsigned char* ptr);
SegrulesFSA* getDefaultSegrulesFSA(const std::map<SegrulesOptions, SegrulesFSA*>& map, const unsigned char* analyzerPtr);
std::vector<uint32_t> getSeparatorsList(const unsigned char* ptr);
void debugMap(const std::map<SegrulesOptions, SegrulesFSA*>& res);

}
#endif	/* SEGRULES_HPP */

