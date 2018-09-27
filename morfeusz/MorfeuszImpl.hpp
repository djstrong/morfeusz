/* 
 * File:   Morfeusz.hpp
 * Author: mlenart
 *
 * Created on November 13, 2013, 5:21 PM
 */

#ifndef MORFEUSZ_HPP
#define	MORFEUSZ_HPP

#include <string>
#include <list>
#include <vector>
#include <map>
#include <set>

#include "morfeusz2.h"

#include "fsa/fsa.hpp"
#include "ChunkBounds.hpp"
#include "InterpsGroup.hpp"
#include "case/CaseConverter.hpp"
#include "charset/CharsetConverter.hpp"
#include "charset/TextReader.hpp"
#include "InterpretedChunk.hpp"
#include "InflexionGraph.hpp"
#include "MorfeuszOptions.hpp"
#include "const.hpp"
#include "Environment.hpp"
#include "ResultsIteratorImpl.hpp"

#include "segrules/segrules.hpp"
#include "segrules/SegrulesFSA.hpp"

#include "deserialization/InterpsGroupsReader.hpp"
#include "deserialization/MorphDeserializer.hpp"

namespace morfeusz {

    class MorfeuszImpl;
    class MorphInterpretation;
    class ResultsIterator;
//    class ResultsIteratorImpl;

    typedef State<InterpsGroupsReader> StateType;

    /**
     * Performs morphological analysis (analyze methods) and syntesis (generate methods).
     * 
     * It is NOT thread-safe
     * but it is possible to use separate Morfeusz instance for each concurrent thread.
     */
    class MorfeuszImpl : public Morfeusz {
    public:
        MorfeuszImpl(const std::string& dictName, MorfeuszUsage usage);

        virtual ~MorfeuszImpl();

        std::string getDictID() const;

        std::string getDictCopyright() const;

        ResultsIterator* analyse(const std::string& text) const;
        
        ResultsIterator* analyse(const char* text) const;

        void analyse(const std::string& text, std::vector<MorphInterpretation>& result) const;

        void generate(const std::string& lemma, std::vector<MorphInterpretation>& result) const;

        void generate(const std::string& lemma, int tagId, std::vector<MorphInterpretation>& result) const;

        void setCharset(Charset encoding);
        
        Charset getCharset() const;

        void setAggl(const std::string& aggl);
        
        std::string getAggl() const;

        void setPraet(const std::string& praet);
        
        std::string getPraet() const;

        void setCaseHandling(CaseHandling caseHandling);
        
        CaseHandling getCaseHandling() const;

        void setTokenNumbering(TokenNumbering tokenNumbering);
        
        TokenNumbering getTokenNumbering() const;
        
        void setWhitespaceHandling(WhitespaceHandling whitespaceHandling);
        
        WhitespaceHandling getWhitespaceHandling() const;
        
        void setDebug(bool debug);

        const IdResolver& getDefaultAnalyzerTagset() const;

        const IdResolver& getDefaultGeneratorTagset() const;
        
        ResultsIterator* analyseWithCopy(const char* text) const;
        
        const IdResolver& getIdResolver() const;

        void setDictionary(const std::string& dictName);
        
        const std::set<std::string>& getAvailableAgglOptions() const;
        
        const std::set<std::string>& getAvailablePraetOptions() const;
        
        Morfeusz* clone() const;
        
        friend class ResultsIteratorImpl;
    
    private:

        void processOneWord(
                const Environment& env,
                TextReader& reader,
                int startNodeNum,
                std::vector<MorphInterpretation>& result,
                bool insideIgnHandler = false) const;
        
        void analyseOneWord(
                TextReader& reader,
                std::vector<MorphInterpretation>& results) const;
        
        void adjustTokensCounter() const;
        
        void doProcessOneWord(
                const Environment& env,
                TextReader& reader,
                const SegrulesState& segrulesState) const;

        void processInterpsGroup(
                const Environment& env,
                const TextReader& reader,
                bool isAtWhitespace,
                const SegrulesState& segrulesState,
                const std::string& homonymId,
                const InterpsGroup& ig) const;

        void processInterpretedChunk(
                const Environment& env,
                const TextReader& reader,
                bool isAtWhitespace,
                bool caseMatches,
                const SegrulesState& newState,
                InterpretedChunk& ic) const;
        
        void processWhitespacesChunk(
                TextReader& reader,
                int startNodeNum,
                std::vector<MorphInterpretation>& results) const;
        
        /**
         * Read whitespaces at beginning of the word. 
         * Add whitespace MorphIntepretations to results vector 
         * iff whitespace handling is set to KEEP
         * 
         * @param reader
         * @param startNodeNum
         * @param results
         * @return - true iff some whitespace interpretations were added to the results vector
         */
        bool handleWhitespacesAtBeginning(
            const Environment& env,
            TextReader& reader, 
            int startNodeNum, 
            std::vector<MorphInterpretation>& results) const;
        
        /**
         * Skips whitespaces at end of the word
         * iff whitespaceHandling set to APPEND.
         *
         * @param env
         * @param reader
         * @return pointer to chunk end (possibly after some whitespaces)
         */
        const char* handleWhitespacesAtEnd(
            const Environment& env,
            TextReader& reader) const;

        void handleIgnChunk(
                const Environment& env,
                const ChunkBounds& chunkBounds,
                int startNodeNum,
                std::vector<MorphInterpretation>& results) const;

        void appendIgnotiumToResults(
                const Environment& env,
                const ChunkBounds& chunkBounds,
                int startNodeNum,
                std::vector<MorphInterpretation>& results) const;
        
        void ensureIsAnalyzer() const;
        
        void ensureIsGenerator() const;
        
        const Environment& getAnyEnvironment() const;
        
        void doSetDictionary(const std::string& dictName);

        std::string currDictionary;
        MorfeuszUsage usage;
        Environment analyzerEnv;
        Environment generatorEnv;
        MorfeuszOptions options;
        mutable std::vector<InterpretedChunk> accum;
        mutable int notMatchingCaseSegs;
        mutable InflexionGraph graph;
        mutable int nextNodeNum;
    };

}

#endif	/* MORFEUSZ_HPP */

