/*
 * File:   TestMorfeusz.cpp
 * Author: lennyn
 *
 * Created on Jun 27, 2014, 1:03:19 PM
 */

#include "TestMorfeusz.hpp"

#include <cstdio>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <iostream>

CPPUNIT_TEST_SUITE_REGISTRATION(TestMorfeusz);

using namespace std;
using namespace morfeusz;

TestMorfeusz::TestMorfeusz() {
}

TestMorfeusz::~TestMorfeusz() {
}

void TestMorfeusz::setUp() {
    cerr << "SET UP" << endl;
    morfeusz = Morfeusz::createInstance(BOTH_ANALYSE_AND_GENERATE);
    cerr << "SET UP done" << endl;
}

void TestMorfeusz::tearDown() {
    cerr << "TEAR DOWN" << endl;
    //    delete morfeusz;
}

void TestMorfeusz::testAnalyzeIterate1() {
    cerr << "testAnalyzeIterate1" << endl;
    ResultsIterator* it = morfeusz->analyse("AAAAbbbbCCCC");
    CPPUNIT_ASSERT(it->hasNext());
    CPPUNIT_ASSERT_EQUAL(string("AAAAbbbbCCCC"), it->peek().orth);
    CPPUNIT_ASSERT_EQUAL(string("AAAAbbbbCCCC"), it->next().orth);
    CPPUNIT_ASSERT(!it->hasNext());
    CPPUNIT_ASSERT_THROW(it->peek(), std::out_of_range);
    CPPUNIT_ASSERT_THROW(it->next(), std::out_of_range);
    delete it;
}

void TestMorfeusz::testAnalyzeIterateWithWhitespaceHandlingKEEP() {
    cerr << "testAnalyzeIterateWithWhitespaceHandlingKEEP" << endl;
    morfeusz->setWhitespaceHandling(KEEP_WHITESPACES);
    ResultsIterator* it = morfeusz->analyse(" AAAAbbbbCCCC  DDDDeeee.\t");

    CPPUNIT_ASSERT(it->hasNext());
    CPPUNIT_ASSERT_EQUAL(string(" "), it->next().orth);

    CPPUNIT_ASSERT(it->hasNext());
    CPPUNIT_ASSERT_EQUAL(string("AAAAbbbbCCCC"), it->next().orth);

    CPPUNIT_ASSERT(it->hasNext());
    CPPUNIT_ASSERT_EQUAL(string("  "), it->next().orth);

    CPPUNIT_ASSERT(it->hasNext());
    CPPUNIT_ASSERT_EQUAL(string("DDDDeeee"), it->next().orth);

    CPPUNIT_ASSERT(it->hasNext());
    CPPUNIT_ASSERT_EQUAL(string("."), it->next().orth);

    CPPUNIT_ASSERT(it->hasNext());
    CPPUNIT_ASSERT_EQUAL(string("\t"), it->next().orth);

    CPPUNIT_ASSERT(!it->hasNext());
    CPPUNIT_ASSERT_THROW(it->peek(), std::out_of_range);
    CPPUNIT_ASSERT_THROW(it->next(), std::out_of_range);
    delete it;
}

void TestMorfeusz::testAnalyzeIterateWithWhitespaceHandlingAPPEND() {
    cerr << "testAnalyzeIterateWithWhitespaceHandlingAPPEND" << endl;
    morfeusz->setWhitespaceHandling(APPEND_WHITESPACES);
    ResultsIterator* it = morfeusz->analyse(" AAAAbbbbCCCC  DDDDeeee.\t");

    CPPUNIT_ASSERT(it->hasNext());
    CPPUNIT_ASSERT_EQUAL(string(" AAAAbbbbCCCC  "), it->next().orth);

    CPPUNIT_ASSERT(it->hasNext());
    CPPUNIT_ASSERT_EQUAL(string("DDDDeeee"), it->next().orth);

    CPPUNIT_ASSERT(it->hasNext());
    CPPUNIT_ASSERT_EQUAL(string(".\t"), it->next().orth);

    CPPUNIT_ASSERT(!it->hasNext());
    CPPUNIT_ASSERT_THROW(it->peek(), std::out_of_range);
    CPPUNIT_ASSERT_THROW(it->next(), std::out_of_range);
    delete it;
}

void TestMorfeusz::testAnalyzeVector1() {
    cerr << "testAnalyzeVector1" << endl;
    vector<MorphInterpretation> res;
    morfeusz->analyse("AAAAbbbbCCCC", res);
    CPPUNIT_ASSERT_EQUAL((size_t) 1, res.size());
    CPPUNIT_ASSERT_EQUAL(string("AAAAbbbbCCCC"), res[0].orth);
    CPPUNIT_ASSERT_EQUAL(string("AAAAbbbbCCCC"), res[0].lemma);
}

static inline string prepareErrorneusDictFile(const string& dictName) {
    string filename = dictName + "-a.dict";
    ofstream out;
    out.open(filename.c_str());
    out << "asfasdfa" << endl;
    return filename;
}

void TestMorfeusz::testOpenInvalidDict() {
    cerr << "testOpenInvalidDict" << endl;
    string dictName = "asdfasdfasdfa";
    string filename = prepareErrorneusDictFile(dictName);
    morfeusz->dictionarySearchPaths.push_front(".");
    cerr << "still alive..." << endl;
    CPPUNIT_ASSERT_THROW(morfeusz->setDictionary(dictName), FileFormatException);
    remove(filename.c_str());
}

void TestMorfeusz::testOpenNonExistentDict() {
    //    cerr << "testOpenNonExistentFile" << endl;
    //    string filename(tmpnam(NULL));
    CPPUNIT_ASSERT_THROW(morfeusz->setDictionary("asdfasdfa"), MorfeuszException);
}

void TestMorfeusz::testSetInvalidAgglOption() {
    cerr << "testSetInvalidAgglOption" << endl;
    CPPUNIT_ASSERT_THROW(morfeusz->setAggl("asdfasdfa"), MorfeuszException);
}

void TestMorfeusz::testSetInvalidPraetOption() {
    cerr << "testSetInvalidPraetOption" << endl;
    CPPUNIT_ASSERT_THROW(morfeusz->setPraet("asdfasdfa"), MorfeuszException);
}

void TestMorfeusz::testWhitespaceHandlingKEEP() {
    cerr << "testWhitespaceHandlingKEEP" << endl;
    vector<MorphInterpretation> res;
    morfeusz->setWhitespaceHandling(KEEP_WHITESPACES);
    morfeusz->analyse("  AAAAbbbbCCCC DDDDeeee\t", res);
    CPPUNIT_ASSERT_EQUAL((size_t) 5, res.size());
    CPPUNIT_ASSERT_EQUAL(string("  "), res[0].orth);
    CPPUNIT_ASSERT_EQUAL(string("  "), res[0].lemma);
    CPPUNIT_ASSERT_EQUAL(1, res[0].tagId);
    CPPUNIT_ASSERT_EQUAL(string("AAAAbbbbCCCC"), res[1].orth);
    CPPUNIT_ASSERT_EQUAL(string("AAAAbbbbCCCC"), res[1].lemma);
    CPPUNIT_ASSERT_EQUAL(0, res[1].tagId);
    CPPUNIT_ASSERT_EQUAL(string(" "), res[2].orth);
    CPPUNIT_ASSERT_EQUAL(string(" "), res[2].lemma);
    CPPUNIT_ASSERT_EQUAL(1, res[2].tagId);
    CPPUNIT_ASSERT_EQUAL(string("DDDDeeee"), res[3].orth);
    CPPUNIT_ASSERT_EQUAL(string("DDDDeeee"), res[3].lemma);
    CPPUNIT_ASSERT_EQUAL(0, res[3].tagId);
    CPPUNIT_ASSERT_EQUAL(string("\t"), res[4].orth);
    CPPUNIT_ASSERT_EQUAL(string("\t"), res[4].lemma);
    CPPUNIT_ASSERT_EQUAL(1, res[4].tagId);
}

void TestMorfeusz::testWhitespaceHandlingAPPEND() {
    cerr << "testWhitespaceHandlingAPPEND" << endl;
    vector<MorphInterpretation> res;
    morfeusz->setWhitespaceHandling(APPEND_WHITESPACES);
    morfeusz->analyse("  AAAAbbbbCCCC DDDDeeee\t", res);
    CPPUNIT_ASSERT_EQUAL((size_t) 2, res.size());
    CPPUNIT_ASSERT_EQUAL(string("  AAAAbbbbCCCC "), res[0].orth);
    CPPUNIT_ASSERT_EQUAL(string("AAAAbbbbCCCC"), res[0].lemma);
    CPPUNIT_ASSERT_EQUAL(0, res[0].tagId);
    CPPUNIT_ASSERT_EQUAL(string("DDDDeeee\t"), res[1].orth);
    CPPUNIT_ASSERT_EQUAL(string("DDDDeeee"), res[1].lemma);
    CPPUNIT_ASSERT_EQUAL(0, res[1].tagId);
}

void TestMorfeusz::testDefaultWhitespaceHandling() {
    CPPUNIT_ASSERT_EQUAL(morfeusz::SKIP_WHITESPACES, morfeusz->getWhitespaceHandling());
}

void TestMorfeusz::testDefaultCaseHandling() {
    CPPUNIT_ASSERT_EQUAL(morfeusz::CONDITIONALLY_CASE_SENSITIVE, morfeusz->getCaseHandling());
    
}
void TestMorfeusz::testDefaultTokenNumbering() {
    CPPUNIT_ASSERT_EQUAL(morfeusz::SEPARATE_NUMBERING, morfeusz->getTokenNumbering());
    
}
