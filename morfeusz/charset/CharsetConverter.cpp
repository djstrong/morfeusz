
#include <vector>
#include <iterator>
#include <algorithm>
#include <inttypes.h>
#include <iostream>
#include "deserialization/endianness.hpp"
#include "utf8.h"
#include "utf8sqlite.hpp"
#include "CharsetConverter.hpp"
#include "conversion_tables.hpp"

using namespace std;

namespace morfeusz {

const char DEFAULT_UNDEFINED_CHAR = static_cast<char> (0xF7);

string CharsetConverter::toString(const vector<uint32_t>& codepoints) const {
    string res;
    for (unsigned int i = 0; i < codepoints.size(); i++) {
        this->append(codepoints[i], res);
    }
    return res;
}

CharsetConverter::~CharsetConverter() {

}

static inline void iterateThroughInvalidUtf8Sequence(const char*& it, const char* end) {
    uint32_t ignoredChar;
    while (it != end && utf8::internal::validate_next(it, end, ignoredChar) != utf8::internal::UTF8_OK) {
        it++;
    }
}

const UTF8CharsetConverter& UTF8CharsetConverter::getInstance() {
    static UTF8CharsetConverter instance;
    return instance;
}

UTF8CharsetConverter::UTF8CharsetConverter() {}

uint32_t UTF8CharsetConverter::next(const char*& it, const char* end) const {
    return readUTF8((const unsigned char*&) it, (const unsigned char*) end);
}

void UTF8CharsetConverter::append(uint32_t cp, string& result) const {
    utf8::unchecked::append(cp, back_inserter(result));
}

static vector<char> reverseArray(const uint32_t* array) {
    vector<char> res;
    unsigned char c = 0;
    do {
        uint32_t codepoint = array[c];
        res.resize(max(static_cast<uint32_t> (res.size()), codepoint + 1), DEFAULT_UNDEFINED_CHAR);
        res[codepoint] = static_cast<char> (c);
        c++;
    }
    while (c != 255);
    return res;
}

OneByteCharsetConverter::OneByteCharsetConverter(const uint32_t* array)
: array(array),
codepoint2Char(reverseArray(array)) {
}

// TODO - sprawdzanie zakresu

uint32_t OneByteCharsetConverter::next(const char*& it, const char* end) const {
    return this->array[static_cast<unsigned char> (*it++)];
}

void OneByteCharsetConverter::append(uint32_t cp, std::string& result) const {
    if (cp < this->codepoint2Char.size()) {
        result.push_back(this->codepoint2Char[cp]);
    }
    else {
        result.push_back(DEFAULT_UNDEFINED_CHAR);
    }
}

const ISO8859_2_CharsetConverter& ISO8859_2_CharsetConverter::getInstance() {
    static ISO8859_2_CharsetConverter instance;
    return instance;
}

ISO8859_2_CharsetConverter::ISO8859_2_CharsetConverter()
: OneByteCharsetConverter(ISO_8859_2_TO_CODEPOINT) {
}

const Windows_1250_CharsetConverter& Windows_1250_CharsetConverter::getInstance() {
    static Windows_1250_CharsetConverter instance;
    return instance;
}

Windows_1250_CharsetConverter::Windows_1250_CharsetConverter()
: OneByteCharsetConverter(WINDOWS_1250_TO_CODEPOINT) {
}

const CP852_CharsetConverter& CP852_CharsetConverter::getInstance() {
    static CP852_CharsetConverter instance;
    return instance;
}

CP852_CharsetConverter::CP852_CharsetConverter()
: OneByteCharsetConverter(CP852_TO_CODEPOINT) {
}

string CharsetConverter::fromUTF8(const string& input) const {
    string res;
    const char* currInput = input.c_str();
    const char* inputEnd = input.c_str() + input.length();
    while (currInput != inputEnd) {
        uint32_t cp = utf8::next(currInput, inputEnd);
        this->append(cp, res);
    }
    return res;
}

string CharsetConverter::toUTF8(const string& input) const {
    string res;
    const char* currInput = input.c_str();
    const char* inputEnd = input.c_str() + input.length();
    while (currInput != inputEnd) {
        uint32_t cp = this->next(currInput, inputEnd);
        UTF8CharsetConverter::getInstance().append(cp, res);
    }
    return res;
}

string UTF8CharsetConverter::fromUTF8(const string& input) const {
    return input;
}

string UTF8CharsetConverter::toUTF8(const string& input) const {
    return input;
}

}
