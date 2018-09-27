/* 
 * File:   Environment.hpp
 * Author: mlenart
 *
 * Created on 22 styczeń 2014, 12:08
 */

#ifndef ENVIRONMENT_HPP
#define	ENVIRONMENT_HPP

#include <vector>
#include <set>

#include "case/CaseConverter.hpp"
#include "charset/CharsetConverter.hpp"
#include "fsa/fsa.hpp"
#include "segrules/segrules.hpp"
#include "const.hpp"
#include "IdResolverImpl.hpp"
#include "InterpsGroup.hpp"
#include "case/CasePatternHelper.hpp"
#include "deserialization/InterpsGroupsReader.hpp"
#include "Dictionary.hpp"

namespace morfeusz {

class InterpretedChunksDecoder;
class CasePatternHelper;
struct InterpsGroup;
typedef FSA<InterpsGroupsReader> FSAType;

/**
 * This class contains data required for morphological analysis/synthesis.
 * It contains references to dictionary automaton, charset converter, tagset data etc.
 * All of these can be changed by setters, changing Morfeusz behavior (different dictionary, charset, and other options).
 */
class Environment {
public:
    /**
     * Creates default environment with given initial charset, processor type (analyzer/generator) and default dictionary data ptr.
     * 
     * @param charset
     * @param morfeuszProcessor
     * @param fileStartPtr
     */
    Environment(const std::string& dictName, MorfeuszProcessorType morfeuszProcessor, bool usable);
    
    /**
     * Sets charset for this environment.
     * 
     * @param charset
     */
    void setCharset(Charset charset);
    
    /**
     * Sets case sensitivity options.
     * 
     * @param caseSensitive - if true, interpretations not matching case will be discarded.
     */
    void setCaseSensitive(bool caseSensitive);
    
    /**
     * Gets charset converter that is currently used by this environment.
     * Changed by setting charset.
     * 
     * @return - reference to charset converter.
     */
    const CharsetConverter& getCharsetConverter() const;
    
    /**
     * Returns case converter that is currently used by this environment.
     * Changed by setting case sensitivity option.
     * 
     * @return - reference to case converter.
     */
    const CaseConverter& getCaseConverter() const;
    
//    /**
//     * Sets new tagset for this environment.
//     * 
//     * @param tagset
//     */
//    void setTagset(IdResolverImpl& tagset);
    
    /**
     * Gets currently used tagset.
     * 
     * @return 
     */
    const IdResolverImpl& getIdResolver() const;
    
    const Dictionary* getCurrentDictionary() const;
    
    /**
     * Sets dictionary by this environment.
     * 
     * @param dict - pointer to the dictionary
     */
    void setDictionary(const Dictionary* dict);
    
    /**
     * Sets segmentation rules option.
     * 
     * @param option
     * @param value
     */
    void setSegrulesOption(const std::string& option, const std::string& value);
    
    /**
     * Gets current segmentation rules option.
     * 
     * @param option
     * @param value
     */
    std::string getSegrulesOption(const std::string& option) const;
    
    /**
     * Gets segmentation rules automaton.
     * 
     * @return 
     */
    const SegrulesFSA& getCurrentSegrulesFSA() const;
    
    /**
     * Gets dictionary automaton.
     * 
     * @return 
     */
    const FSAType& getFSA() const;
    
    /**
     * Returns decoder that converts interpretations to external format.
     * @return 
     */
    const InterpretedChunksDecoder& getInterpretedChunksDecoder() const;
    
    /**
     * Gets processor type (info if this is analyzer or generator environment)
     * @return 
     */
    MorfeuszProcessorType getProcessorType() const;
    
    /**
     * Return current case pattern helper
     * 
     * @return 
     */
    const CasePatternHelper& getCasePatternHelper() const;
    
    /**
     * Returns true iff given codepoint denotes a separator char for ign handling.
     * @param codepoint
     * @return 
     */
    bool isSeparator(uint32_t codepoint) const;
    
    const std::set<std::string>& getAvailableAgglOptions() const;
    
    const std::set<std::string>& getAvailablePraetOptions() const;
    
    /**
     * Returns true iff this Environment has dictionary attached to it
     * @return 
     */
    bool isUsable() const;
    
    virtual ~Environment();
private:
    bool usable;
    const CharsetConverter* currentCharsetConverter;
    const CaseConverter caseConverter;
    
    const Dictionary* dictionary;
    IdResolverImpl idResolver;

    SegrulesOptions currSegrulesOptions;
    const SegrulesFSA* currSegrulesFSA;
    
    const InterpretedChunksDecoder* chunksDecoder;
    MorfeuszProcessorType processorType;
    CasePatternHelper* casePatternHelper;
    
    const CharsetConverter* getCharsetConverter(Charset charset) const;
    
    std::string getAvailableOptionsAsString(const std::string& option) const;
};

}

#endif	/* ENVIRONMENT_HPP */

