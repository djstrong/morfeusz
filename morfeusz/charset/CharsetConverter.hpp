/* 
 * File:   EncodingConverter.hpp
 * Author: mlenart
 *
 * Created on 14 listopad 2013, 17:28
 */

#ifndef ENCODINGCONVERTER_HPP
#define	ENCODINGCONVERTER_HPP

#include <inttypes.h>
#include <string>
#include <vector>
#include <map>

namespace morfeusz {

class CharsetConverter {
public:
//    uint32_t peek(const char* it, const char* end) const;
    virtual uint32_t next(const char*& it, const char* end) const = 0;
    virtual void append(uint32_t cp, std::string& result) const = 0;
    virtual std::string fromUTF8(const std::string& input) const;
    virtual std::string toUTF8(const std::string& input) const;

    std::string toString(const std::vector<uint32_t>& codepoints) const;

    virtual ~CharsetConverter();
private:
};

class UTF8CharsetConverter : public CharsetConverter {
public:

    static const UTF8CharsetConverter& getInstance();

    uint32_t next(const char*& it, const char* end) const;
    void append(uint32_t cp, std::string& result) const;
    std::string fromUTF8(const std::string& input) const;
    std::string toUTF8(const std::string& input) const;
private:
    UTF8CharsetConverter();
    UTF8CharsetConverter(const UTF8CharsetConverter&); // do not implement
    void operator=(const UTF8CharsetConverter&); // do not implement
};

/*
 * Converter that uses a simple conversion table
 */
class OneByteCharsetConverter : public CharsetConverter {
public:
    explicit OneByteCharsetConverter(const uint32_t* array);
    uint32_t next(const char*& it, const char* end) const;
    void append(uint32_t cp, std::string& result) const;
private:
    const uint32_t* array;
    const std::vector<char> codepoint2Char;
};

class ISO8859_2_CharsetConverter : public OneByteCharsetConverter {
public:
    static const ISO8859_2_CharsetConverter& getInstance();
private:
    ISO8859_2_CharsetConverter();
    ISO8859_2_CharsetConverter(const ISO8859_2_CharsetConverter&); // do not implement
    void operator=(const ISO8859_2_CharsetConverter&); // do not implement
};

class Windows_1250_CharsetConverter : public OneByteCharsetConverter {
public:
    static const Windows_1250_CharsetConverter& getInstance();
private:
    Windows_1250_CharsetConverter();
    Windows_1250_CharsetConverter(const Windows_1250_CharsetConverter&); // do not implement
    void operator=(const Windows_1250_CharsetConverter&); // do not implement
};

class CP852_CharsetConverter : public OneByteCharsetConverter {
public:
    static const CP852_CharsetConverter& getInstance();
private:
    CP852_CharsetConverter();
    CP852_CharsetConverter(const CP852_CharsetConverter&); // do not implement
    void operator=(const CP852_CharsetConverter&); // do not implement
};

}

#endif	/* ENCODINGCONVERTER_HPP */

