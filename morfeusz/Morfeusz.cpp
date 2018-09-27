
#include <string>
#include "const.hpp"
#include "morfeusz2.h"
#include "MorfeuszImpl.hpp"

namespace morfeusz {

    using namespace std;

    string Morfeusz::getVersion() {
        return MORFEUSZ2_VERSION;
    }

    string Morfeusz::getDefaultDictName() {
        return MORFEUSZ_DEFAULT_DICT_NAME;
    }
    
    string Morfeusz::getCopyright() {
        return COPYRIGHT_TEXT;
    }

    Morfeusz* Morfeusz::createInstance(MorfeuszUsage usage) {
        return new MorfeuszImpl(Morfeusz::getDefaultDictName(), usage);
    }

    Morfeusz* Morfeusz::createInstance(const std::string& dictName, MorfeuszUsage usage) {
        return new MorfeuszImpl(dictName, usage);
    }

    Morfeusz::~Morfeusz() {
    }

    static list<string> initializeDictionarySearchPaths() {
        list<string> res;
#ifdef MORFEUSZ_DICTIONARY_SEARCH_PATH1
        res.push_back(MORFEUSZ_DICTIONARY_SEARCH_PATH1);
#endif
#ifdef MORFEUSZ_DICTIONARY_SEARCH_PATH2
        res.push_back(MORFEUSZ_DICTIONARY_SEARCH_PATH2);
#endif
#ifdef MORFEUSZ_DICTIONARY_SEARCH_PATH3
        res.push_back(MORFEUSZ_DICTIONARY_SEARCH_PATH3);
#endif
        return res;
    }

    list<string> Morfeusz::dictionarySearchPaths(initializeDictionarySearchPaths());

}
