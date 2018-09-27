/* 
 * File:   Dictionary.cpp
 * Author: lennyn
 * 
 * Created on August 8, 2014, 3:15 PM
 */

#include "Dictionary.hpp"
#include "charset/CharsetConverter.hpp"
#include "deserialization/MorphDeserializer.hpp"

using namespace std;

namespace morfeusz {

    static Deserializer<InterpsGroupsReader>& initializeDeserializer(MorfeuszProcessorType processorType) {
        static Deserializer<InterpsGroupsReader> *analyzerDeserializer
                = new MorphDeserializer();
        static Deserializer<InterpsGroupsReader> *generatorDeserializer
                = new MorphDeserializer();
        return *(processorType == ANALYZER ? analyzerDeserializer : generatorDeserializer);
    }

    static set<string> getAvailableOptions(const map<SegrulesOptions, SegrulesFSA*> segrulesFSAsMap, const string& option) {
        set<string> res;
        for (
                map<SegrulesOptions, SegrulesFSA*>::const_iterator it = segrulesFSAsMap.begin();
                it != segrulesFSAsMap.end();
                ++it) {
            res.insert(it->first.find(option)->second);
        }
        return res;
    }
    
    Dictionary* Dictionary::getEmpty() {
        static Dictionary* dict = new Dictionary();
        return dict;
    }
    
    Dictionary::Dictionary()
    : fsa(NULL),
    id(),
    copyright(),
    idResolver(),
    separatorsList(),
    segrulesFSAsMap(),
    defaultSegrulesOptions(),
    defaultSegrulesFSA(NULL),
    availableAgglOptions(),
    availablePraetOptions() {
    }

    Dictionary::Dictionary(const unsigned char* fsaFileStartPtr, MorfeuszProcessorType processorType)
    : fsa(FSAType::getFSA(fsaFileStartPtr, initializeDeserializer(processorType))),
    id(),
    copyright(),
    idResolver(fsaFileStartPtr, &UTF8CharsetConverter::getInstance()),
    separatorsList(getSeparatorsList(fsaFileStartPtr)),
    segrulesFSAsMap(createSegrulesFSAsMap(fsaFileStartPtr)),
    defaultSegrulesOptions(getDefaultSegrulesOptions(fsaFileStartPtr)),
    defaultSegrulesFSA(getDefaultSegrulesFSA(this->segrulesFSAsMap, fsaFileStartPtr)),
    availableAgglOptions(getAvailableOptions(segrulesFSAsMap, "aggl")),
    availablePraetOptions(getAvailableOptions(segrulesFSAsMap, "praet")) {
        const unsigned char* currPtr = getEpiloguePtr(fsaFileStartPtr) + 4;
        this->id = readString(currPtr);
        this->copyright = readString(currPtr);
    }
    
    bool Dictionary::isCompatibleWith(const Dictionary& other) const {
        return this->id == other.id
                && this->idResolver.isCompatibleWith(other.idResolver)
                && this->availableAgglOptions == other.availableAgglOptions
                && this->availablePraetOptions == other.availablePraetOptions
                && this->defaultSegrulesOptions == other.defaultSegrulesOptions
                && this->separatorsList == other.separatorsList;
    }
}
