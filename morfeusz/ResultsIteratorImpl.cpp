/* 
 * File:   ResultsIteratorImpl.cpp
 * Author: mlenart
 * 
 * Created on 26 czerwiec 2014, 14:37
 */

#include "ResultsIteratorImpl.hpp"

#include <cstring>
#include <stdexcept>

namespace morfeusz {

    ResultsIteratorImpl::ResultsIteratorImpl(const MorfeuszImpl& morfeusz, const char* text, const char* textEnd, bool isOwnerOfText)
    :
    morfeusz(morfeusz),
    text(text),
    isOwnerOfText(isOwnerOfText),
    reader(text, textEnd, morfeusz.analyzerEnv),
    buffer(),
    bufferIterator(buffer.begin()) {
    }

    ResultsIteratorImpl::~ResultsIteratorImpl() {
        if (isOwnerOfText) {
            delete text;
        }
    }

    bool ResultsIteratorImpl::hasNext() {
        return bufferIterator != buffer.end() || tryToReadIntoBuffer();
    }

    const MorphInterpretation& ResultsIteratorImpl::peek() {
        ensureHasNext();
        return *bufferIterator;
    }

    MorphInterpretation ResultsIteratorImpl::next() {
        ensureHasNext();
        return *bufferIterator++;
    }

    void ResultsIteratorImpl::ensureHasNext() {
        if (!hasNext()) {
            throw std::out_of_range("No more interpretations available to ResultsIterator");
        }
    }

    bool ResultsIteratorImpl::tryToReadIntoBuffer() {
        assert(bufferIterator == buffer.end());
        buffer.resize(0);
        if (!reader.isAtEnd()) {
            morfeusz.analyseOneWord(reader, buffer);
        }
        bufferIterator = buffer.begin();
        return bufferIterator != buffer.end();
    }
}

