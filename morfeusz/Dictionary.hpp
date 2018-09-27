/* 
 * File:   Dictionary.hpp
 * Author: lennyn
 *
 * Created on August 8, 2014, 3:15 PM
 */

#ifndef DICTIONARY_HPP
#define	DICTIONARY_HPP

#include <string>
#include <vector>
#include <map>
#include <set>
#include "IdResolverImpl.hpp"
#include "fsa/fsa.hpp"
#include "segrules/segrules.hpp"
#include "const.hpp"
#include "deserialization/InterpsGroupsReader.hpp"

namespace morfeusz {
    
    typedef FSA<InterpsGroupsReader> FSAType;

    struct Dictionary {
        Dictionary(const unsigned char* ptr, MorfeuszProcessorType processorType);
        
        Dictionary();
        
        bool isCompatibleWith(const Dictionary& other) const;
        
        static Dictionary* getEmpty();
        
        FSAType* fsa;
        std::string id;
        std::string copyright;
        IdResolverImpl idResolver;
        std::vector<uint32_t> separatorsList;
        std::map<SegrulesOptions, SegrulesFSA*> segrulesFSAsMap;
        SegrulesOptions defaultSegrulesOptions;
        SegrulesFSA* defaultSegrulesFSA;
        std::set<std::string> availableAgglOptions;
        std::set<std::string> availablePraetOptions;
    };
}

#endif	/* DICTIONARY_HPP */

