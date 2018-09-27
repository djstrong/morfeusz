/* 
 * File:   Morfeusz.cpp
 * Author: mlenart
 * 
 * Created on November 13, 2013, 5:21 PM
 */

#include <string>
#include <iostream>
#include <vector>
#include <cstring>
#include <stdexcept>
#include "fsa/fsa.hpp"
#include "utils.hpp"
#include "MorfeuszImpl.hpp"
#include "deserialization/morphInterps/InterpretedChunksDecoder.hpp"
#include "charset/CharsetConverter.hpp"
#include "charset/charset_utils.hpp"
#include "case/CaseConverter.hpp"
#include "segrules/segrules.hpp"
#include "const.hpp"
#include "charset/utf8.h"
#include "ChunkBounds.hpp"
#include "DictionariesRepository.hpp"

// TODO - konstruktor kopiujący działający Tak-Jak-Trzeba

using namespace std;

namespace morfeusz {

    static MorfeuszOptions createDefaultOptions() {
        MorfeuszOptions res;
        res.caseHandling = CONDITIONALLY_CASE_SENSITIVE;
        res.encoding = UTF8;
        res.tokenNumbering = SEPARATE_NUMBERING;
        res.whitespaceHandling = SKIP_WHITESPACES;
        res.debug = false;
        return res;
    }
    
    static string debugInterpsGroup(unsigned char type, const char* startPtr, const char* endPtr) {
        stringstream res;
        res << "(" << (int) type << ", " << string(startPtr, endPtr) << "), ";
        return res.str();
    }

    static string debugAccum(vector<InterpretedChunk>& accum) {
        stringstream res;
        for (unsigned int i = 0; i < accum.size(); i++) {
            res << debugInterpsGroup(accum[i].segmentType, accum[i].textNoPrefixesStartPtr, accum[i].textEndPtr);
        }
        return res.str();
    }

    static void doShiftOrth(InterpretedChunk& from, InterpretedChunk& to) {
        to.prefixChunks.swap(from.prefixChunks); // from.prefixChunks are ignored anyway. Will swap them back in doUnshiftOrth
        to.prefixChunks.push_back(from);
        to.textStartPtr = from.textStartPtr;
        from.orthWasShifted = true;
    }
    
    static void doUnshiftOrth(InterpretedChunk& from, InterpretedChunk& to) {
        to.prefixChunks.swap(from.prefixChunks);
        from.prefixChunks.pop_back();
    }

    static void feedStateDirectly(
            const FSAType& fsa,
            StateType& state,
            const char* inputStart,
            const char* inputEnd) {
        const char* currInput = inputStart;
        while (currInput != inputEnd && !state.isSink()) {
            state.proceedToNext(fsa, *currInput++);
        }
    }

    static void feedStateIndirectly(
            const FSAType& fsa,
            StateType& state,
            uint32_t codepoint) {
        std::string chars;
        UTF8CharsetConverter::getInstance().append(codepoint, chars);
        for (unsigned int i = 0; i < chars.length() && !state.isSink(); i++) {
            state.proceedToNext(fsa, chars[i]);
        }
    }

    static void feedState(
            const Environment& env,
            StateType& state,
            TextReader& reader) {
        if (reader.peek() == reader.normalizedPeek() && &env.getCharsetConverter() == &UTF8CharsetConverter::getInstance()) {
            feedStateDirectly(env.getFSA(), state, reader.getCurrPtr(), reader.getNextPtr());
        } else {
            feedStateIndirectly(env.getFSA(), state, reader.normalizedPeek());
        }
    }

    static InterpretedChunk createChunk(
            const InterpsGroup& ig,
            const TextReader& reader,
            bool shiftOrth,
            const string& homonymId) {
        const unsigned char* interpsEndPtr = ig.ptr + ig.size;
        InterpretedChunk ic;
        ic.segmentType = ig.type;
        ic.textStartPtr = reader.getWordStartPtr(); // may be changed later in doShiftOrth(...) function
        ic.textNoPrefixesStartPtr = ic.textStartPtr;
        ic.textEndPtr = homonymId.empty() ? reader.getCurrPtr() : reader.getCurrPtr() - homonymId.length() - 1;
        ic.interpsGroupPtr = ig.ptr;
        ic.interpsEndPtr = interpsEndPtr;
        ic.shiftOrth = shiftOrth;
        ic.orthWasShifted = false;
        ic.requiredHomonymId = homonymId;
        ic.codepointsNum = reader.getCodepointsRead();
        ic.forceIgnoreCase = false;
        return ic;
    }

    MorfeuszImpl::MorfeuszImpl(const string& dictName, MorfeuszUsage usage)
    : currDictionary(dictName),
    usage(usage),
    analyzerEnv(dictName, ANALYZER, usage != GENERATE_ONLY),
    generatorEnv(dictName, GENERATOR, usage != ANALYSE_ONLY),
    options(createDefaultOptions()),
    accum(),
    notMatchingCaseSegs(0),
    graph(),
    nextNodeNum(0) {
        analyzerEnv.setCaseSensitive(options.caseHandling != IGNORE_CASE);
        generatorEnv.setCaseSensitive(false);
    }

    Morfeusz* MorfeuszImpl::clone() const {
        return new MorfeuszImpl(*this);
    }

    string MorfeuszImpl::getDictID() const {
        return getAnyEnvironment().getCurrentDictionary()->id;
    }
    
    string MorfeuszImpl::getDictCopyright() const {
        return getAnyEnvironment().getCurrentDictionary()->copyright;
    }

    void MorfeuszImpl::setDictionary(const string& dictName) {

        if (dictName != currDictionary) {

            doSetDictionary(dictName);

            currDictionary = dictName;
        }
    }

    void MorfeuszImpl::doSetDictionary(const string& dictName) {
        switch (usage) {
            case BOTH_ANALYSE_AND_GENERATE:
            {
                const Dictionary* analyzerDict = DictionariesRepository::getInstance().getDictionary(dictName, ANALYZER);
                const Dictionary* generatorDict = DictionariesRepository::getInstance().getDictionary(dictName, GENERATOR);
                if (analyzerDict->isCompatibleWith(*generatorDict)) {
                    analyzerEnv.setDictionary(analyzerDict);
                    generatorEnv.setDictionary(generatorDict);
                } 
                else {
                    throw MorfeuszException("Analyzer and generator dictionaries are incompatible");
                }
            }
                break;
            case ANALYSE_ONLY:
                analyzerEnv.setDictionary(DictionariesRepository::getInstance().getDictionary(dictName, ANALYZER));
                break;
            case GENERATE_ONLY:
                generatorEnv.setDictionary(DictionariesRepository::getInstance().getDictionary(dictName, GENERATOR));
                break;
        }
    }

    const Environment& MorfeuszImpl::getAnyEnvironment() const {
        if (analyzerEnv.isUsable()) {
            return analyzerEnv;
        } else {
            return generatorEnv;
        }
    }

    const set<string>& MorfeuszImpl::getAvailableAgglOptions() const {
        return getAnyEnvironment().getAvailableAgglOptions();
    }

    const set<string>& MorfeuszImpl::getAvailablePraetOptions() const {
        return getAnyEnvironment().getAvailablePraetOptions();
    }

    MorfeuszImpl::~MorfeuszImpl() {
    }

    const char* getWordEndPtr(const TextReader& reader, const Environment& env) {
        TextReader tmpReader(reader.getCurrPtr(), reader.getEndPtr(), env);
        while (!tmpReader.isAtEnd() && !tmpReader.isAtWhitespace()) {
            tmpReader.next();
        }
        return tmpReader.getCurrPtr();
    }

    bool MorfeuszImpl::handleWhitespacesAtBeginning(
            const Environment& env,
            TextReader& reader,
            int startNodeNum,
            std::vector<MorphInterpretation>& results) const {
        if (env.getProcessorType() == ANALYZER) {
            switch (options.whitespaceHandling) {
                case KEEP_WHITESPACES:
                {
                    bool res = reader.isAtWhitespace() && !reader.isAtEnd();
                    if (res) {
                        processWhitespacesChunk(reader, startNodeNum, results);
                    }
                    reader.markChunkStartsHere();
                    reader.markWordStartsHere();
                    return res;
                }
                case APPEND_WHITESPACES:
                    reader.markChunkStartsHere();
                    reader.skipWhitespaces();
                    reader.markWordStartsHere();
                    return false;
                case SKIP_WHITESPACES:
                    reader.skipWhitespaces();
                    reader.markChunkStartsHere();
                    reader.markWordStartsHere();
                    return false;
                default:
                    break;
            }
        }

        return false;
    }

    const char* MorfeuszImpl::handleWhitespacesAtEnd(
            const Environment& env,
            TextReader& reader) const {
        if (env.getProcessorType() == ANALYZER
                && options.whitespaceHandling == APPEND_WHITESPACES) {
            reader.skipWhitespaces();
        }
        return reader.getCurrPtr();
    }

    void MorfeuszImpl::processOneWord(
            const Environment& env,
            TextReader& reader,
            int startNodeNum,
            vector<MorphInterpretation>& results,
            bool insideIgnHandler) const {
        if (handleWhitespacesAtBeginning(env, reader, startNodeNum, results)) {
            startNodeNum = results.back().endNode;
        }

        if (reader.isAtEnd()) {
            return;
        }
        accum.resize(0);
        notMatchingCaseSegs = 0;
        graph.clear();

        const SegrulesFSA& segrulesFSA = env.getCurrentSegrulesFSA();

        doProcessOneWord(env, reader, segrulesFSA.initialState);

        while (reader.isInsideAWord()) {
            reader.next();
        }

        ChunkBounds chunkBounds;
        chunkBounds.chunkStartPtr = reader.getChunkStartPtr();
        chunkBounds.wordStartPtr = reader.getWordStartPtr();
        chunkBounds.wordEndPtr = reader.getCurrPtr();
        chunkBounds.chunkEndPtr = handleWhitespacesAtEnd(env, reader);

        if (!graph.empty()) {
            const InterpretedChunksDecoder& interpretedChunksDecoder = env.getInterpretedChunksDecoder();
            int srcNode = startNodeNum;
            const std::vector< std::vector<InflexionGraph::Edge> >& theGraph = graph.getTheGraph();
            size_t initialResultsSize = results.size();
            for (unsigned int i = 0; i < theGraph.size(); i++) {
                const vector<InflexionGraph::Edge>& edges = theGraph[i];
                for (unsigned int j = 0; j < edges.size(); j++) {
                    const InflexionGraph::Edge& e = edges[j];
                    unsigned int targetNode = startNodeNum + e.nextNode;
                    InterpretedChunk ic = e.chunk;
                    ic.chunkStartPtr =
                            ic.textStartPtr == reader.getWordStartPtr()
                            ? reader.getChunkStartPtr()
                            : ic.textStartPtr;
                    ic.chunkEndPtr = 
                            ic.textEndPtr == chunkBounds.wordEndPtr
                            ? chunkBounds.chunkEndPtr
                            : ic.textEndPtr;
                    interpretedChunksDecoder.decode(srcNode, targetNode, ic, results);
                }
                srcNode++;
            }
            if (results.size() == initialResultsSize) {
                this->appendIgnotiumToResults(env, chunkBounds, startNodeNum, results);
            }
        } 
        else if (env.getProcessorType() == ANALYZER
                && !insideIgnHandler) {
            this->handleIgnChunk(env, chunkBounds, startNodeNum, results);
        } 
        else {
            this->appendIgnotiumToResults(env, chunkBounds, startNodeNum, results);
        }
    }

    void MorfeuszImpl::doProcessOneWord(
            const Environment& env,
            TextReader& reader,
            const SegrulesState& segrulesState) const {
        if (this->options.debug) {
            cerr << "----------" << endl;
            cerr << "doProcessOneWord: '" << reader.getCurrPtr() << "', already recognized: " << debugAccum(accum) << endl;
        }
        StateType state = env.getFSA().getInitialState();
        string homonymId;
        while (!reader.isAtWhitespace()) {
            feedState(env, state, reader);
            if (state.isSink()) {
                return;
            }
            reader.next();
            if (env.getProcessorType() == GENERATOR && reader.getCurrPtr() != reader.getEndPtr() && reader.peek() == (uint32_t) HOMONYM_SEPARATOR) {
                homonymId = env.getCharsetConverter().fromUTF8(string(reader.getCurrPtr() + 1, reader.getEndPtr()));
                reader.proceedToEnd();
            }
            if (state.isAccepting()) {
                InterpsGroupsReader& igReader = const_cast<InterpsGroupsReader&> (state.getValue());
                while (igReader.hasNext()) {
                    processInterpsGroup(env, reader, reader.isAtWhitespace(), segrulesState, homonymId, igReader.getNext());
                }
            }
        }
    }

    void MorfeuszImpl::processInterpsGroup(
            const Environment& env,
            const TextReader& reader,
            bool isAtWhitespace,
            const SegrulesState& segrulesState,
            const string& homonymId,
            const InterpsGroup& ig) const {
        if (this->options.debug) {
            std::cerr << "processInterpsGroup, segmentType=" << (int) ig.type << std::endl;
        }
        bool caseMatches = env.getCasePatternHelper().checkInterpsGroupOrthCasePatterns(env, reader.getWordStartPtr(), reader.getCurrPtr(), ig);
        if (caseMatches || options.caseHandling == CONDITIONALLY_CASE_SENSITIVE) {
            SegrulesState newSegrulesState;
            env.getCurrentSegrulesFSA().proceedToNext(ig.type, segrulesState, isAtWhitespace, newSegrulesState);
            if (!newSegrulesState.failed) {
                
                InterpretedChunk ic(
                        createChunk(ig, reader, newSegrulesState.shiftOrthFromPrevious, homonymId));

                processInterpretedChunk(
                        env,
                        reader,
                        isAtWhitespace,
                        caseMatches,
                        newSegrulesState,
                        ic);
            }
            else if (this->options.debug) {
                std::cerr << "NOT ACCEPTING (segmentation)" << debugAccum(accum) << debugInterpsGroup(ig.type, reader.getWordStartPtr(), reader.getCurrPtr()) << std::endl;
            }
        } 
        else if (this->options.debug) {
            std::cerr << "NOT ACCEPTING (case)" << debugAccum(accum) << debugInterpsGroup(ig.type, reader.getWordStartPtr(), reader.getCurrPtr()) << std::endl;
        }
    }

    void MorfeuszImpl::processInterpretedChunk(
            const Environment& env,
            const TextReader& reader,
            bool isAtWhitespace,
            bool caseMatches,
            const SegrulesState& newSegrulesState,
            InterpretedChunk& ic) const {
        bool orthShifted = false;
        if (!accum.empty() && accum.back().shiftOrth) {
            doShiftOrth(accum.back(), ic);
            orthShifted = true;
        }
        if (!caseMatches && options.caseHandling == CONDITIONALLY_CASE_SENSITIVE) {
            notMatchingCaseSegs++;
            ic.forceIgnoreCase = true;
        }
        accum.push_back(ic);
        if (isAtWhitespace) {
            assert(newSegrulesState.accepting);
            if (this->options.debug) {
                cerr << "ACCEPTING " << debugAccum(accum) << " prefixChunks: " << debugAccum(accum.back().prefixChunks) << endl;
            }
            graph.addPath(accum, newSegrulesState.weak || notMatchingCaseSegs > 0);
        } 
        else {
            assert(!newSegrulesState.sink);
            TextReader newReader(reader.getCurrPtr(), reader.getEndPtr(), env);
            doProcessOneWord(env, newReader, newSegrulesState);
        }
        accum.pop_back();
        if (orthShifted) {
            doUnshiftOrth(accum.back(), ic);
        }
        if (!caseMatches && options.caseHandling == CONDITIONALLY_CASE_SENSITIVE) {
            notMatchingCaseSegs--;
        }
    }

    void MorfeuszImpl::processWhitespacesChunk(
            TextReader& reader,
            int startNodeNum,
            std::vector<MorphInterpretation>& results) const {
        string orth(reader.readWhitespacesChunk());
        results.push_back(MorphInterpretation::createWhitespace(startNodeNum, startNodeNum + 1, orth));
    }

    void MorfeuszImpl::handleIgnChunk(
            const Environment& env,
            const ChunkBounds& chunkBounds,
            int startNodeNum,
            std::vector<MorphInterpretation>& results) const {
        const char* currInput = chunkBounds.chunkStartPtr;
        const char* prevInput = currInput;
        uint32_t codepoint = 0x00;
        bool separatorFound = false;
        while (currInput != chunkBounds.chunkEndPtr) {
            prevInput = currInput;
            const char* nonSeparatorInputEnd = prevInput;
            
            // advance through non-separator chars
            do {
                codepoint = env.getCharsetConverter().next(currInput, chunkBounds.chunkEndPtr);
                if (!env.isSeparator(codepoint)) {
                    nonSeparatorInputEnd = currInput;
                }
            } while (currInput != chunkBounds.chunkEndPtr && !env.isSeparator(codepoint));

            // advance through separator chars
            if (env.isSeparator(codepoint)) {
                separatorFound = true;
                if (nonSeparatorInputEnd != prevInput) {
                    // there are some non-separators + some separators

                    int startNode = results.empty() ? startNodeNum : results.back().endNode;
                    // process part before separators
                    TextReader newReader1(prevInput, nonSeparatorInputEnd, env);
                    notMatchingCaseSegs = 0;
                    this->processOneWord(env, newReader1, startNode, results, true);

                    // process separators part
                    if (currInput == chunkBounds.wordEndPtr) {
                        currInput = chunkBounds.chunkEndPtr;
                    }
                    startNode = results.empty() ? startNodeNum : results.back().endNode;
                    TextReader newReader2(nonSeparatorInputEnd, currInput, env);
                    this->processOneWord(env, newReader2, startNode, results, true);
                } else {
                    // there are only separators
                    if (currInput == chunkBounds.wordEndPtr) {
                        currInput = chunkBounds.chunkEndPtr;
                    }
                    int startNode = results.empty() ? startNodeNum : results.back().endNode;
                    TextReader newReader3(prevInput, currInput, env);
                    notMatchingCaseSegs = 0;
                    this->processOneWord(env, newReader3, startNode, results, true);
                }
            }
        }

        // currInput == chunkBounds.chunkEndPtr
        if (!env.isSeparator(codepoint)) {
            if (separatorFound) {
                // process (remaining) non-separators
                int startNode = results.empty() ? startNodeNum : results.back().endNode;
                TextReader newReader4(prevInput, chunkBounds.chunkEndPtr, env);
                this->processOneWord(env, newReader4, startNode, results, true);
            } else {
                // no separators found at all - whole chunk is ign
                this->appendIgnotiumToResults(env, chunkBounds, startNodeNum, results);
            }
        }
    }

    void MorfeuszImpl::appendIgnotiumToResults(
            const Environment& env,
            const ChunkBounds& chunkBounds,
            int startNodeNum,
            std::vector<MorphInterpretation>& results) const {
        string orth(chunkBounds.chunkStartPtr, chunkBounds.chunkEndPtr);
        string lemma(chunkBounds.wordStartPtr, chunkBounds.wordEndPtr);
        results.push_back(MorphInterpretation::createIgn(startNodeNum, startNodeNum + 1, orth, lemma));
    }

    void MorfeuszImpl::analyseOneWord(
            TextReader& reader,
            vector<MorphInterpretation>& results) const {
        this->processOneWord(this->analyzerEnv, reader, nextNodeNum, results);
        if (!results.empty()) {
            nextNodeNum = results.back().endNode;
        }
    }

    void MorfeuszImpl::adjustTokensCounter() const {
        if (options.tokenNumbering == SEPARATE_NUMBERING) {
            nextNodeNum = 0;
        }
    }

    ResultsIterator* MorfeuszImpl::analyse(const string& text) const {

        ensureIsAnalyzer();

        adjustTokensCounter();
        char* textCopy = new char[text.length() + 1];
        strcpy(textCopy, text.c_str());
        return new ResultsIteratorImpl(*this, textCopy, textCopy + text.length(), true);
    }

    ResultsIterator* MorfeuszImpl::analyseWithCopy(const char* text) const {

        ensureIsAnalyzer();

        adjustTokensCounter();
        long n = strlen(text);
        char* textCopy = new char[n + 1];
        strcpy(textCopy, text);
        return new ResultsIteratorImpl(*this, textCopy, textCopy + n, true);
    }

    ResultsIterator* MorfeuszImpl::analyse(const char* text) const {

        ensureIsAnalyzer();

        adjustTokensCounter();
        return new ResultsIteratorImpl(*this, text, text + strlen(text), false);
    }

    void MorfeuszImpl::analyse(const string& text, vector<MorphInterpretation>& results) const {

        ensureIsAnalyzer();

        adjustTokensCounter();
        TextReader reader(text, this->analyzerEnv);
        while (!reader.isAtEnd()) {
            analyseOneWord(reader, results);
        }
    }

    void MorfeuszImpl::generate(const string& lemma, vector<MorphInterpretation>& results) const {

        ensureIsGenerator();

        const char* input = lemma.c_str();
        const char* inputEnd = input + lemma.length();
        int startNode = 0;
        TextReader reader(input, inputEnd, this->generatorEnv);
        this->processOneWord(this->generatorEnv, reader, startNode, results);
        if (reader.getCurrPtr() != reader.getEndPtr()) {
            throw MorfeuszException("Input contains more than one word");
        }
    }

    void MorfeuszImpl::generate(const std::string& lemma, int tagId, vector<MorphInterpretation>& result) const {

        ensureIsGenerator();

        if (tagId >= this->generatorEnv.getIdResolver().getTagsCount()) {
            throw MorfeuszException("Invalid tagId (outside of tagset)");
        }

        vector<MorphInterpretation> partRes;
        this->generate(lemma, partRes);
        for (unsigned int i = 0; i < partRes.size(); i++) {
            // XXX - someday it should be improved
            if (partRes[i].tagId == tagId) {
                result.push_back(partRes[i]);
            }
        }
    }

    void MorfeuszImpl::setCharset(Charset charset) {
        this->options.encoding = charset;
        this->analyzerEnv.setCharset(charset);
        this->generatorEnv.setCharset(charset);
    }
    
    Charset MorfeuszImpl::getCharset() const {
        return this->options.encoding;
    }

    void MorfeuszImpl::setAggl(const std::string& aggl) {
        this->analyzerEnv.setSegrulesOption("aggl", aggl);
        this->generatorEnv.setSegrulesOption("aggl", aggl);
    }
    
    string MorfeuszImpl::getAggl() const {
        return getAnyEnvironment().getSegrulesOption("aggl");
    }

    void MorfeuszImpl::setPraet(const std::string& praet) {
        this->analyzerEnv.setSegrulesOption("praet", praet);
        this->generatorEnv.setSegrulesOption("praet", praet);
    }
    
    string MorfeuszImpl::getPraet() const {
        return getAnyEnvironment().getSegrulesOption("praet");
    }

    void MorfeuszImpl::setCaseHandling(CaseHandling caseHandling) {
        switch (caseHandling) {
            case IGNORE_CASE:
            case CONDITIONALLY_CASE_SENSITIVE:
            case STRICTLY_CASE_SENSITIVE:
                break;
            default:
                throw std::invalid_argument("Invalid caseHandling option");
        }
        this->options.caseHandling = caseHandling;
        this->analyzerEnv.setCaseSensitive(caseHandling != IGNORE_CASE);
    }
    
    CaseHandling MorfeuszImpl::getCaseHandling() const {
        return this->options.caseHandling;
    }

    void MorfeuszImpl::setTokenNumbering(TokenNumbering tokenNumbering) {
        switch (tokenNumbering) {
            case SEPARATE_NUMBERING:
            case CONTINUOUS_NUMBERING:
                break;
            default:
                throw std::invalid_argument("Invalid tokenNumbering option");
        }
        this->options.tokenNumbering = tokenNumbering;
        nextNodeNum = 0;
    }
    
    TokenNumbering MorfeuszImpl::getTokenNumbering() const {
        return this->options.tokenNumbering;
    }

    void MorfeuszImpl::setWhitespaceHandling(WhitespaceHandling whitespaceHandling) {
        switch (whitespaceHandling) {
            case SKIP_WHITESPACES:
            case APPEND_WHITESPACES:
            case KEEP_WHITESPACES:
                break;
            default:
                throw std::invalid_argument("Invalid whitespaceHandling option");
        }
        this->options.whitespaceHandling = whitespaceHandling;
    }
    
    WhitespaceHandling MorfeuszImpl::getWhitespaceHandling() const {
        return this->options.whitespaceHandling;
    }

    void MorfeuszImpl::setDebug(bool debug) {
        this->options.debug = debug;
    }

    const IdResolver& MorfeuszImpl::getIdResolver() const {
        if (this->analyzerEnv.isUsable()) {
            return this->analyzerEnv.getIdResolver();
        } else {
            return this->generatorEnv.getIdResolver();
        }
    }

    void MorfeuszImpl::ensureIsAnalyzer() const {
        if (usage != ANALYSE_ONLY && usage != BOTH_ANALYSE_AND_GENERATE) {
            throw MorfeuszException("Cannot analyse with given Morfeusz instance.");
        }
    }

    void MorfeuszImpl::ensureIsGenerator() const {
        if (usage != GENERATE_ONLY && usage != BOTH_ANALYSE_AND_GENERATE) {
            throw MorfeuszException("Cannot generate with given Morfeusz instance.");
        }
    }
}
