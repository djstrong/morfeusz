/* 
 * File:   DictionariesRepository.cpp
 * Author: lennyn
 * 
 * Created on August 8, 2014, 3:49 PM
 */

#include <fstream>
#include "morfeusz2.h"
#include "DictionariesRepository.hpp"
#ifdef MORFEUSZ_EMBEDDED_DEFAULT_DICT
#include "data/default_fsa.hpp"
#endif
#include "const.hpp"
#include "utils.hpp"

namespace morfeusz {

    using namespace std;

    DictionariesRepository& DictionariesRepository::getInstance() {
        static DictionariesRepository* res = new DictionariesRepository();
        return *res;
    }

    string DictionariesRepository::getDictionaryFilename(const std::string& name, MorfeuszProcessorType processorType) {
        string processorTypeSuffix;
        switch (processorType) {
            case ANALYZER:
                processorTypeSuffix = "-a";
                break;
            case GENERATOR:
                processorTypeSuffix = "-s";
                break;
            default:
                throw MorfeuszException("Internal error: invalid Morfeusz processor type");
        }
        string extensionSuffix = ".dict";
        return name + processorTypeSuffix + extensionSuffix;
    }

    map<std::string, DictionariesRepository::RepositoryEntry> DictionariesRepository::getDefaultEntriesMap() {
        map<std::string, DictionariesRepository::RepositoryEntry> res;
#ifdef MORFEUSZ_EMBEDDED_DEFAULT_DICT
        DictionariesRepository::RepositoryEntry defaultEntry;
        defaultEntry.analyzerDictionary = new Dictionary(DEFAULT_FSA, ANALYZER);
        defaultEntry.generatorDictionary = new Dictionary(DEFAULT_SYNTH_FSA, GENERATOR);
        res[Morfeusz::getDefaultDictName()] = defaultEntry;
#endif
        return res;
    }

    DictionariesRepository::DictionariesRepository()
    : entriesMap(getDefaultEntriesMap()) {
    }

    const Dictionary* DictionariesRepository::getDefaultDictionary(MorfeuszProcessorType processorType) {
        return this->getDictionary(Morfeusz::getDefaultDictName(), processorType);
    }

    Dictionary* DictionariesRepository::RepositoryEntry::getDictionary(MorfeuszProcessorType processorType) const {
        switch (processorType) {
            case ANALYZER:
                return analyzerDictionary;
            case GENERATOR:
                return generatorDictionary;
            default:
                throw MorfeuszException("Internal error: invalid Morfeusz processor type");
        }
    }
    
    static void raiseException(const string& name, MorfeuszProcessorType processorType) {
        string processorTypeStr;
        switch (processorType) {
            case ANALYZER:
                processorTypeStr = "analyzer";
                break;
            case GENERATOR:
                processorTypeStr = "generator";
                break;
            default:
                throw MorfeuszException("Internal error: invalid Morfeusz processor type");
        }
        throw MorfeuszException("Failed to load "+processorTypeStr+" dictionary \""+name+"\"");
    }

    const Dictionary* DictionariesRepository::getDictionary(const string& name, MorfeuszProcessorType processorType) {
        if (!hasLoadedDictionary(name, processorType) && !tryToLoadDictionary(name, processorType)) {
            raiseException(name, processorType);
        }
        return entriesMap.find(name)->second.getDictionary(processorType);
    }

    bool DictionariesRepository::hasLoadedDictionary(const string& name, MorfeuszProcessorType processorType) const {
        if (entriesMap.find(name) == entriesMap.end()) {
            return false;
        } else {
            const DictionariesRepository::RepositoryEntry& entry = entriesMap.find(name)->second;
            return entry.getDictionary(processorType) != NULL;
        }
    }

    static bool fileExists(const string& fileName) {
        ifstream infile(fileName.c_str());
        return infile.good();
    }

    bool DictionariesRepository::tryToLoadDictionary(const string& name, MorfeuszProcessorType processorType) {
        list<string>::const_iterator it = Morfeusz::dictionarySearchPaths.begin();
        while (it != Morfeusz::dictionarySearchPaths.end()) {
            string dirpath = *it;
            string filepath = dirpath + FILESYSTEM_PATH_SEPARATOR + getDictionaryFilename(name, processorType);
            if (fileExists(filepath)) {
                doLoadDictionary(name, filepath, processorType);
                return true;
            }
            ++it;
        }
        return false;
    }

    void DictionariesRepository::doLoadDictionary(const std::string& dictName, const std::string& filepath, MorfeuszProcessorType processorType) {
        const unsigned char* ptr(readFile<unsigned char>(filepath.c_str()));
        if (entriesMap.find(dictName) == entriesMap.end()) {
            DictionariesRepository::RepositoryEntry entry;
            entry.analyzerDictionary = NULL;
            entry.generatorDictionary = NULL;
            entriesMap[dictName] = entry;
        }
        DictionariesRepository::RepositoryEntry& entry = entriesMap.find(dictName)->second;
        Dictionary* dict = new Dictionary(ptr, processorType);
        switch (processorType) {
            case ANALYZER:
                entry.analyzerDictionary = dict;
                break;
            case GENERATOR:
                entry.generatorDictionary = dict;
                break;
            default:
                throw MorfeuszException("Internal error: invalid Morfeusz processor type");
        }
    }
}
