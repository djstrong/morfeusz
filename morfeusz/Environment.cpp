/* 
 * File:   Environment.cpp
 * Author: mlenart
 * 
 * Created on 22 styczeń 2014, 12:08
 */

#include <vector>
#include <algorithm>
#include <string>
#include "DictionariesRepository.hpp"
#include "Environment.hpp"
#include "deserialization/MorphDeserializer.hpp"
#include "deserialization/morphInterps/InterpretedChunksDecoder.hpp"
#include "deserialization/morphInterps/InterpretedChunksDecoder4Analyzer.hpp"
#include "deserialization/morphInterps/InterpretedChunksDecoder4Generator.hpp"
#include "DictionariesRepository.hpp"

namespace morfeusz {

    using namespace std;

    Environment::Environment(const string& dictName, MorfeuszProcessorType processorType, bool usable)
    : usable(usable),
    currentCharsetConverter(getCharsetConverter(DEFAULT_MORFEUSZ_CHARSET)),
    caseConverter(),
    dictionary(usable
    ? DictionariesRepository::getInstance().getDictionary(dictName, processorType)
    : Dictionary::getEmpty()),
    idResolver(dictionary->idResolver),
    currSegrulesOptions(dictionary->defaultSegrulesOptions),
    currSegrulesFSA(dictionary->defaultSegrulesFSA),
    chunksDecoder(
    processorType == ANALYZER
    ? (InterpretedChunksDecoder*) new InterpretedChunksDecoder4Analyzer(*this)
    : (InterpretedChunksDecoder*) new InterpretedChunksDecoder4Generator(*this)),
    processorType(processorType),
    casePatternHelper(new CasePatternHelper()) {
    }

    const CharsetConverter* Environment::getCharsetConverter(Charset charset) const {
        switch (charset) {
            case UTF8:
                return &UTF8CharsetConverter::getInstance();
            case ISO8859_2:
                return &ISO8859_2_CharsetConverter::getInstance();
            case CP1250:
                return &Windows_1250_CharsetConverter::getInstance();
            case CP852:
                return &CP852_CharsetConverter::getInstance();
            default:
                throw MorfeuszException("invalid charset");
        }
    }

    Environment::~Environment() {
        delete this->chunksDecoder;
        delete this->casePatternHelper;
    }

    void Environment::setCharset(Charset charset) {
        this->currentCharsetConverter = this->getCharsetConverter(charset);
        this->idResolver.setCharsetConverter(currentCharsetConverter);
    }

    const CharsetConverter& Environment::getCharsetConverter() const {
        return *this->currentCharsetConverter;
    }

    const CaseConverter& Environment::getCaseConverter() const {
        return this->caseConverter;
    }

    const IdResolverImpl& Environment::getIdResolver() const {
        return this->idResolver;
    }

    const SegrulesFSA& Environment::getCurrentSegrulesFSA() const {
        return *(this->currSegrulesFSA);
    }

    const FSAType& Environment::getFSA() const {
        return *(this->dictionary->fsa);
    }

    const InterpretedChunksDecoder& Environment::getInterpretedChunksDecoder() const {
        return *(this->chunksDecoder);
    }

    void Environment::setSegrulesOption(const std::string& option, const std::string& value) {
        if (this->isUsable()) {
            if (this->currSegrulesOptions.find(option) == this->currSegrulesOptions.end()) {
                throw MorfeuszException("Invalid segmentation option '" + option + "'");
            }
            SegrulesOptions prevOptions = this->currSegrulesOptions;
            this->currSegrulesOptions[option] = value;
            if (this->dictionary->segrulesFSAsMap.find(this->currSegrulesOptions) == this->dictionary->segrulesFSAsMap.end()) {
                this->currSegrulesOptions = prevOptions;

                throw MorfeuszException("Invalid \"" + option + "\" option: \"" + value + "\". Possible values: " + getAvailableOptionsAsString(option));
            }
            this->currSegrulesFSA = this->dictionary->segrulesFSAsMap.find(this->currSegrulesOptions)->second;
        }
    }
    
    string Environment::getSegrulesOption(const std::string& option) const {
        if (this->currSegrulesOptions.find(option) == this->currSegrulesOptions.end()) {
                throw MorfeuszException("Invalid segmentation option '" + option + "'");
            }
        return this->currSegrulesOptions.find(option)->second;
    }

    MorfeuszProcessorType Environment::getProcessorType() const {
        return this->processorType;
    }

    void Environment::setCaseSensitive(bool caseSensitive) {
        this->casePatternHelper->setCaseSensitive(caseSensitive);
    }

    const CasePatternHelper& Environment::getCasePatternHelper() const {
        return *this->casePatternHelper;
    }

    bool Environment::isSeparator(uint32_t codepoint) const {
        return binary_search(
                this->dictionary->separatorsList.begin(),
                this->dictionary->separatorsList.end(),
                codepoint);
    }

    bool Environment::isUsable() const {
        return usable;
    }
    
    const Dictionary* Environment::getCurrentDictionary() const {
        return this->dictionary;
    }


    void Environment::setDictionary(const Dictionary* dict) {
        this->dictionary = dict;
        idResolver = dictionary->idResolver;
        this->idResolver.setCharsetConverter(currentCharsetConverter);
        currSegrulesOptions = dictionary->defaultSegrulesOptions;
        currSegrulesFSA = dictionary->defaultSegrulesFSA;
    }

    string Environment::getAvailableOptionsAsString(const string& option) const {
        const set<string>* options;
        if (option == "aggl") {
            options = &dictionary->availableAgglOptions;
        } else {
            options = &dictionary->availablePraetOptions;
        }
        string res;
        set<string>::const_iterator it = options->begin();
        while (it != options->end()) {
            if (!res.empty()) {
                res += ", ";
            }
            res += '"';
            res += *it;
            res += '"';
            ++it;
        }
        return res;
    }

    const set<string>& Environment::getAvailableAgglOptions() const {
        return this->dictionary->availableAgglOptions;
    }

    const set<string>& Environment::getAvailablePraetOptions() const {
        return this->dictionary->availablePraetOptions;
    }
}
