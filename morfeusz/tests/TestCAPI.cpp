/*
 * File:   TestCAPI.cpp
 * Author: lennyn
 *
 * Created on Jun 27, 2014, 12:49:12 PM
 */

#include "TestCAPI.hpp"
#include <string>
#include <iostream>
#include "morfeusz2_c.h"

using namespace std;


CPPUNIT_TEST_SUITE_REGISTRATION(TestCAPI);

TestCAPI::TestCAPI() {
}

TestCAPI::~TestCAPI() {
}

void TestCAPI::setUp() {
}

void TestCAPI::tearDown() {
}

void TestCAPI::testTwoSimpleInvocations() {
    cerr << "testTwoSimpleInvocations" << endl;

    char* text = "AAaaBBbbCCcc DDDD.";
    InterpMorf* results = morfeusz_analyse(text);
    CPPUNIT_ASSERT_EQUAL(0, results[0].p);
    CPPUNIT_ASSERT_EQUAL(1, results[0].k);
    CPPUNIT_ASSERT_EQUAL(string("AAaaBBbbCCcc"), string(results[0].forma));
    CPPUNIT_ASSERT_EQUAL(string("AAaaBBbbCCcc"), string(results[0].haslo));
    CPPUNIT_ASSERT_EQUAL(string("ign"), string(results[0].interp));

    CPPUNIT_ASSERT_EQUAL(1, results[1].p);
    CPPUNIT_ASSERT_EQUAL(2, results[1].k);
    CPPUNIT_ASSERT_EQUAL(string("DDDD"), string(results[1].forma));
    CPPUNIT_ASSERT_EQUAL(string("DDDD"), string(results[1].haslo));
    CPPUNIT_ASSERT_EQUAL(string("ign"), string(results[1].interp));

    CPPUNIT_ASSERT_EQUAL(2, results[2].p);
    CPPUNIT_ASSERT_EQUAL(3, results[2].k);
    CPPUNIT_ASSERT_EQUAL(string("."), string(results[2].forma));
    CPPUNIT_ASSERT_EQUAL(string("."), string(results[2].haslo));
    //    CPPUNIT_ASSERT_EQUAL(string("ign"), string(results[2].interp));
    CPPUNIT_ASSERT_EQUAL(-1, results[3].p);

    char* text1 = "EEeeFFff";
    results = morfeusz_analyse(text1);
    CPPUNIT_ASSERT_EQUAL(0, results[0].p);
    CPPUNIT_ASSERT_EQUAL(1, results[0].k);
    CPPUNIT_ASSERT_EQUAL(string("EEeeFFff"), string(results[0].forma));
    CPPUNIT_ASSERT_EQUAL(string("EEeeFFff"), string(results[0].haslo));
    CPPUNIT_ASSERT_EQUAL(string("ign"), string(results[0].interp));

    CPPUNIT_ASSERT_EQUAL(-1, results[1].p);
}

void TestCAPI::testWhitespaceKEEP() {
    cerr << "testWhitespaceKEEP" << endl;


    CPPUNIT_ASSERT_EQUAL(1, morfeusz_set_option(MORFOPT_WHITESPACE, MORFEUSZ_KEEP_WHITESPACE));

    char* text = "AAaaBBbbCCcc  .";
    InterpMorf* results = morfeusz_analyse(text);
    CPPUNIT_ASSERT_EQUAL(0, results[0].p);
    CPPUNIT_ASSERT_EQUAL(1, results[0].k);
    CPPUNIT_ASSERT_EQUAL(string("AAaaBBbbCCcc"), string(results[0].forma));
    CPPUNIT_ASSERT_EQUAL(string("AAaaBBbbCCcc"), string(results[0].haslo));
    CPPUNIT_ASSERT_EQUAL(string("ign"), string(results[0].interp));

    CPPUNIT_ASSERT_EQUAL(1, results[1].p);
    CPPUNIT_ASSERT_EQUAL(2, results[1].k);
    CPPUNIT_ASSERT_EQUAL(string("  "), string(results[1].forma));
    CPPUNIT_ASSERT_EQUAL(string("  "), string(results[1].haslo));
    //    CPPUNIT_ASSERT_EQUAL(string("ign"), string(results[2].interp));

    CPPUNIT_ASSERT_EQUAL(2, results[2].p);
    CPPUNIT_ASSERT_EQUAL(3, results[2].k);
    CPPUNIT_ASSERT_EQUAL(string("."), string(results[2].forma));
    CPPUNIT_ASSERT_EQUAL(string("."), string(results[2].haslo));
    //    CPPUNIT_ASSERT_EQUAL(string("ign"), string(results[2].interp));

    CPPUNIT_ASSERT_EQUAL(-1, results[3].p);
}

void TestCAPI::testWhitespaceAPPEND() {
    cerr << "testWhitespaceAPPEND" << endl;

    CPPUNIT_ASSERT_EQUAL(1, morfeusz_set_option(MORFOPT_WHITESPACE, MORFEUSZ_APPEND_WHITESPACE));

    char* text = "AAaaBBbbCCcc  .";
    InterpMorf* results = morfeusz_analyse(text);
    CPPUNIT_ASSERT_EQUAL(0, results[0].p);
    CPPUNIT_ASSERT_EQUAL(1, results[0].k);
    CPPUNIT_ASSERT_EQUAL(string("AAaaBBbbCCcc  "), string(results[0].forma));
    CPPUNIT_ASSERT_EQUAL(string("AAaaBBbbCCcc"), string(results[0].haslo));
    CPPUNIT_ASSERT_EQUAL(string("ign"), string(results[0].interp));

    CPPUNIT_ASSERT_EQUAL(1, results[1].p);
    CPPUNIT_ASSERT_EQUAL(2, results[1].k);
    CPPUNIT_ASSERT_EQUAL(string("."), string(results[1].forma));
    CPPUNIT_ASSERT_EQUAL(string("."), string(results[1].haslo));
    //    CPPUNIT_ASSERT_EQUAL(string("ign"), string(results[2].interp));

    CPPUNIT_ASSERT_EQUAL(-1, results[2].p);
}

void TestCAPI::testEncodingUTF8() {
    cerr << "testEncodingUTF8" << endl;

    unsigned char text[] = {'z', 'a', /* ż */ 197, 188, /* ó */ 195, 179, '\0'};
    char* actualText = (char*) text;
    CPPUNIT_ASSERT_EQUAL(1, morfeusz_set_option(MORFOPT_ENCODING, MORFEUSZ_CP1250));
    InterpMorf* results = morfeusz_analyse(actualText);
    CPPUNIT_ASSERT_EQUAL(0, results[0].p);
    CPPUNIT_ASSERT_EQUAL(1, results[0].k);
    CPPUNIT_ASSERT_EQUAL(string(actualText), string(results[0].forma));
    CPPUNIT_ASSERT_EQUAL(string(actualText), string(results[0].haslo));
    CPPUNIT_ASSERT_EQUAL(string("ign"), string(results[0].interp));
    CPPUNIT_ASSERT_EQUAL(-1, results[2].p);
}

void TestCAPI::testTokenNumberingCONTINUOUS() {
    cerr << "testTokenNumberingCONTINUOUS" << endl;


    CPPUNIT_ASSERT_EQUAL(1, morfeusz_set_option(MORFOPT_WHITESPACE, MORFEUSZ_SKIP_WHITESPACE));
    CPPUNIT_ASSERT_EQUAL(1, morfeusz_set_option(MORFOPT_TOKEN_NUMBERING, MORFEUSZ_CONTINUOUS_TOKEN_NUMBERING));

    InterpMorf* results = morfeusz_analyse("aaaabbbb bbbbcccc.");
    CPPUNIT_ASSERT_EQUAL(0, results[0].p);
    CPPUNIT_ASSERT_EQUAL(1, results[0].k);
    CPPUNIT_ASSERT_EQUAL(string("aaaabbbb"), string(results[0].forma));

    CPPUNIT_ASSERT_EQUAL(1, results[1].p);
    CPPUNIT_ASSERT_EQUAL(2, results[1].k);
    CPPUNIT_ASSERT_EQUAL(string("bbbbcccc"), string(results[1].forma));

    CPPUNIT_ASSERT_EQUAL(2, results[2].p);
    CPPUNIT_ASSERT_EQUAL(3, results[2].k);
    CPPUNIT_ASSERT_EQUAL(string("."), string(results[2].forma));

    CPPUNIT_ASSERT_EQUAL(-1, results[3].p);

    results = morfeusz_analyse(const_cast<char*>("ccccdddd"));
    CPPUNIT_ASSERT_EQUAL(3, results[0].p);
    CPPUNIT_ASSERT_EQUAL(4, results[0].k);
    CPPUNIT_ASSERT_EQUAL(string("ccccdddd"), string(results[0].forma));

    CPPUNIT_ASSERT_EQUAL(-1, results[1].p);
}

void TestCAPI::testEncodingISO8859_2() {
    cerr << "testEncodingISO8859_2" << endl;

    CPPUNIT_ASSERT_EQUAL(1, morfeusz_set_option(MORFOPT_ENCODING, MORFEUSZ_ISO8859_2));
}

void TestCAPI::testEncodingCP1250() {
    cerr << "testEncodingCP1250" << endl;

    CPPUNIT_ASSERT_EQUAL(1, morfeusz_set_option(MORFOPT_ENCODING, MORFEUSZ_CP1250));
}

void TestCAPI::testEncodingCP852() {
    cerr << "testEncodingCP852" << endl;

    CPPUNIT_ASSERT_EQUAL(1, morfeusz_set_option(MORFOPT_ENCODING, MORFEUSZ_CP852));
}

void TestCAPI::testWrongWhitespaceOption() {
    cerr << "testWrongWhitespaceOption" << endl;

    CPPUNIT_ASSERT_EQUAL(0, morfeusz_set_option(MORFOPT_WHITESPACE, 666777));
}

void TestCAPI::testWrongEncodingOption() {
    cerr << "testWrongEncodingOption" << endl;

    CPPUNIT_ASSERT_EQUAL(0, morfeusz_set_option(MORFOPT_ENCODING, 666777));
}

void TestCAPI::testWrongCaseOption() {
    cerr << "testWrongCaseOption" << endl;

    CPPUNIT_ASSERT_EQUAL(0, morfeusz_set_option(MORFOPT_CASE, 666777));
}

void TestCAPI::testWrongTokenNumberingOption() {
    cerr << "testWrongTokenNumberingOption" << endl;

    CPPUNIT_ASSERT_EQUAL(0, morfeusz_set_option(MORFOPT_TOKEN_NUMBERING, 666777));
}

