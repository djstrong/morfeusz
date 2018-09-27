/* 
 * File:   DictionariesRepository.hpp
 * Author: lennyn
 *
 * Created on August 8, 2014, 3:49 PM
 */

#ifndef DICTIONARIESREPOSITORY_HPP
#define	DICTIONARIESREPOSITORY_HPP

#include "morfeusz2.h"
#include "Dictionary.hpp"
#include "const.hpp"
#include <map>
#include <string>
#include <list>

namespace morfeusz {

    class DictionariesRepository {
    public:
        
        DictionariesRepository();
        
        const Dictionary* getDictionary(const std::string& name, MorfeuszProcessorType processorType);
        
        const Dictionary* getDefaultDictionary(MorfeuszProcessorType processorType);
        
        static std::string getDictionaryFilename(const std::string& name, MorfeuszProcessorType processorType);
        
        static DictionariesRepository& getInstance();
        
    private:
        
        struct RepositoryEntry {
            RepositoryEntry(): analyzerDictionary(NULL), generatorDictionary(NULL) {}
            Dictionary* analyzerDictionary;
            Dictionary* generatorDictionary;
            Dictionary* getDictionary(MorfeuszProcessorType processorType) const;
        };
        
        bool hasLoadedDictionary(const std::string& name, MorfeuszProcessorType processorType) const;
        
        bool tryToLoadDictionary(const std::string& name, MorfeuszProcessorType processorType);
        
        void doLoadDictionary(const std::string& dictName, const std::string& filepath, MorfeuszProcessorType processorType);
        
        static std::map<std::string, DictionariesRepository::RepositoryEntry> getDefaultEntriesMap();

        
        std::map<std::string, RepositoryEntry> entriesMap;
    };
}

#endif	/* DICTIONARIESREPOSITORY_HPP */

