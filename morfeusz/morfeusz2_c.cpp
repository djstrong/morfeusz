
#include <vector>
#include <string>
#include <cstdio>
#include <iostream>

#include "morfeusz2_c.h"
#include "morfeusz2.h"
#include "c_api/ResultsManager.hpp"

using namespace std;
using namespace morfeusz;

static Morfeusz* getMorfeuszInstance() {
    static Morfeusz* morfeuszInstance = Morfeusz::createInstance(ANALYSE_ONLY);
    return morfeuszInstance;
}

extern "C" DLLIMPORT
char* morfeusz_about() {
    return const_cast<char*> (MORFEUSZ2_VERSION);
}

extern "C" DLLIMPORT
InterpMorf* morfeusz_analyse(char *tekst) {
    Morfeusz* morfeuszInstance = getMorfeuszInstance();
    static ResultsManager resultsManager(morfeuszInstance);
    static vector<MorphInterpretation> results;
    results.clear();
    string input(tekst);
    morfeuszInstance->analyse(input, results);
    return resultsManager.convertResults(results);
}

static inline int setEncodingOption(int value) {
    Morfeusz* morfeuszInstance = getMorfeuszInstance();
    switch (value) {
        case MORFEUSZ_UTF_8:
            morfeuszInstance->setCharset(UTF8);
            return 1;
        case MORFEUSZ_ISO8859_2:
            morfeuszInstance->setCharset(ISO8859_2);
            return 1;
        case MORFEUSZ_CP1250:
            morfeuszInstance->setCharset(CP1250);
            return 1;
        case MORFEUSZ_CP852:
            morfeuszInstance->setCharset(CP852);
            return 1;
        default:
            fprintf(stderr, "Wrong encoding option %d\n", value);
            return 0;
    }
}

static inline int setWhitespaceOption(int value) {
    Morfeusz* morfeuszInstance = getMorfeuszInstance();
    switch (value) {
        case MORFEUSZ_KEEP_WHITESPACE:
            morfeuszInstance->setWhitespaceHandling(KEEP_WHITESPACES);
            return 1;
        case MORFEUSZ_APPEND_WHITESPACE:
            morfeuszInstance->setWhitespaceHandling(APPEND_WHITESPACES);
            return 1;
        case MORFEUSZ_SKIP_WHITESPACE:
            morfeuszInstance->setWhitespaceHandling(SKIP_WHITESPACES);
            return 1;
        default:
            fprintf(stderr, "Wrong whitespace option %d\n", value);
            return 0;
    }
}

static inline int setCaseOption(int value) {
    Morfeusz* morfeuszInstance = getMorfeuszInstance();
    switch (value) {
        case MORFEUSZ_STRICT_CASE:
            morfeuszInstance->setCaseHandling(STRICTLY_CASE_SENSITIVE);
            return 1;
        case MORFEUSZ_WEAK_CASE:
            morfeuszInstance->setCaseHandling(CONDITIONALLY_CASE_SENSITIVE);
            return 1;
        case MORFEUSZ_IGNORE_CASE:
            morfeuszInstance->setCaseHandling(IGNORE_CASE);
            return 1;
        default:
            fprintf(stderr, "Wrong case option %d\n", value);
            return 0;
    }
}

static inline int setTokenNumberingOption(int value) {
    Morfeusz* morfeuszInstance = getMorfeuszInstance();
    switch (value) {
        case MORFEUSZ_CONTINUOUS_TOKEN_NUMBERING:
            morfeuszInstance->setTokenNumbering(CONTINUOUS_NUMBERING);
            return 1;
        case MORFEUSZ_SEPARATE_TOKEN_NUMBERING:
            morfeuszInstance->setTokenNumbering(SEPARATE_NUMBERING);
            return 1;
        default:
            fprintf(stderr, "Wrong case option %d\n", value);
            return 0;
    }
}

extern "C" DLLIMPORT
int morfeusz_set_option(int option, int value) {
    switch (option) {
        case MORFOPT_ENCODING:
            return setEncodingOption(value);
        case MORFOPT_WHITESPACE:
            return setWhitespaceOption(value);
        case MORFOPT_CASE:
            return setCaseOption(value);
        case MORFOPT_TOKEN_NUMBERING:
            return setTokenNumberingOption(value);
        default:
            fprintf(stderr, "Wrong option %d\n", option);
            return 0;
    }
    return 1;
}

