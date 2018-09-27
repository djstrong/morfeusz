/* 
 * File:   TextReader.cpp
 * Author: lennyn
 * 
 * Created on May 28, 2014, 11:43 AM
 */

#include "TextReader.hpp"
#include "charset_utils.hpp"

using namespace std;

namespace morfeusz {

    TextReader::TextReader(
            const char* inputStart,
            const char* inputEnd,
            const Environment& env)
    : codepointsNum(0),
    chunkStartPtr(inputStart),
    wordStartPtr(inputStart),
    currPtr(inputStart),
    inputEnd(inputEnd),
    env(env),
    knowsAboutWhitespace(false),
    atWhitespace(false),
    peekIsRead(false),
    thePeek(0x00),
    theNormalizedPeek(0x00),
    ptrAfterThePeek(NULL) {
    }

    TextReader::TextReader(const std::string& text, const Environment& env)
    : codepointsNum(0),
    chunkStartPtr(text.c_str()),
    wordStartPtr(text.c_str()),
    currPtr(text.c_str()),
    inputEnd(text.c_str() + text.length()),
    env(env),
    knowsAboutWhitespace(false),
    atWhitespace(false),
    peekIsRead(false),
    thePeek(0x00),
    theNormalizedPeek(0x00),
    ptrAfterThePeek(NULL) {

    }

    void TextReader::markWordStartsHere() {
        codepointsNum = 0;
        wordStartPtr = currPtr;
    }
    
    void TextReader::markChunkStartsHere() {
        chunkStartPtr = currPtr;
    }

    const char* TextReader::getWordStartPtr() const {
        return wordStartPtr;
    }
    
    const char* TextReader::getChunkStartPtr() const {
        return chunkStartPtr;
    }

    const char* TextReader::getCurrPtr() const {
        return currPtr;
    }

    const char* TextReader::getNextPtr() {
        if (!peekIsRead) {
            peek();
        }
        return ptrAfterThePeek;
    }

    const char* TextReader::getEndPtr() const {
        return inputEnd;
    }

    int TextReader::getCodepointsRead() const {
        return codepointsNum;
    }

    bool TextReader::isAtEnd() const {
        return currPtr == inputEnd;
    }

    bool TextReader::isAtWhitespace() {
        if (isAtEnd()) {
            return true;
        }
        else {
            if (!peekIsRead) {
                peek();
            }
            return atWhitespace;
        }
    }

    bool TextReader::isInsideAWord() {
        return !isAtEnd() && !isAtWhitespace();
    }

    uint32_t TextReader::peek() {
        if (peekIsRead) {
            return thePeek;
        }
        else {
            ptrAfterThePeek = currPtr;
            thePeek = env.getCharsetConverter().next(ptrAfterThePeek, inputEnd);
            theNormalizedPeek = env.getProcessorType() == ANALYZER
                    ? env.getCaseConverter().toLower(thePeek)
                    : thePeek;
            atWhitespace = isWhitespace(thePeek);
            peekIsRead = true;
            return thePeek;
        }
    }

    uint32_t TextReader::normalizedPeek() {
        if (!peekIsRead) {
            peek();
        }
        return theNormalizedPeek;
    }

    uint32_t TextReader::next() {
        if (!peekIsRead) {
            peek();
        }
        currPtr = ptrAfterThePeek;
        peekIsRead = false;
        knowsAboutWhitespace = false;
        codepointsNum++;
        return thePeek;
    }

    void TextReader::skipWhitespaces() {
        while (!isAtEnd() && isAtWhitespace()) {
            next();
        }
    }

    string TextReader::readWhitespacesChunk() {
        const char* startPtr = currPtr;
        while (!isAtEnd() && isAtWhitespace()) {
            next();
        }
        string res(startPtr, currPtr);
        return res;
    }

    void TextReader::proceedToEnd() {
        while (!isAtEnd()) {
            next();
        }
    }

    TextReader::~TextReader() {

    }

}
