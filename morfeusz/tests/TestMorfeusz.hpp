/*
 * File:   TestMorfeusz.hpp
 * Author: lennyn
 *
 * Created on Jun 27, 2014, 1:03:19 PM
 */

#ifndef TESTMORFEUSZ_HPP
#define	TESTMORFEUSZ_HPP

#include <cppunit/extensions/HelperMacros.h>

#include "morfeusz2.h"

class TestMorfeusz : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(TestMorfeusz);

    CPPUNIT_TEST(testAnalyzeIterate1);
    CPPUNIT_TEST(testAnalyzeIterateWithWhitespaceHandlingKEEP);
    CPPUNIT_TEST(testAnalyzeIterateWithWhitespaceHandlingAPPEND);
    CPPUNIT_TEST(testAnalyzeVector1);
    CPPUNIT_TEST(testOpenInvalidDict);
    CPPUNIT_TEST(testOpenNonExistentDict);
    CPPUNIT_TEST(testSetInvalidAgglOption);
    CPPUNIT_TEST(testSetInvalidPraetOption);
    CPPUNIT_TEST(testWhitespaceHandlingKEEP);
    CPPUNIT_TEST(testWhitespaceHandlingAPPEND);
    CPPUNIT_TEST(testDefaultWhitespaceHandling);
    CPPUNIT_TEST(testDefaultCaseHandling);
    CPPUNIT_TEST(testDefaultTokenNumbering);

    CPPUNIT_TEST_SUITE_END();

public:
    TestMorfeusz();
    virtual ~TestMorfeusz();
    void setUp();
    void tearDown();

private:
    void testAnalyzeIterate1();
    void testAnalyzeIterateWithWhitespaceHandlingKEEP();
    void testAnalyzeIterateWithWhitespaceHandlingAPPEND();
    void testAnalyzeVector1();
    void testOpenInvalidDict();
    void testOpenNonExistentDict();
    void testSetInvalidAgglOption();
    void testSetInvalidPraetOption();
    void testWhitespaceHandlingKEEP();
    void testWhitespaceHandlingAPPEND();
    void testDefaultWhitespaceHandling();
    void testDefaultCaseHandling();
    void testDefaultTokenNumbering();
    
    morfeusz::Morfeusz* morfeusz;
};

#endif	/* TESTMORFEUSZ_HPP */

