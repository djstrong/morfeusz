/*
 * File:   TestCAPI.hpp
 * Author: lennyn
 *
 * Created on Jun 27, 2014, 12:49:12 PM
 */

#ifndef TESTCAPI_HPP
#define	TESTCAPI_HPP

#include <cppunit/extensions/HelperMacros.h>

class TestCAPI : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(TestCAPI);
    
    CPPUNIT_TEST(testTwoSimpleInvocations);
    CPPUNIT_TEST(testWhitespaceKEEP);
    CPPUNIT_TEST(testWhitespaceAPPEND);
    CPPUNIT_TEST(testEncodingUTF8);
    CPPUNIT_TEST(testEncodingISO8859_2);
    CPPUNIT_TEST(testEncodingCP1250);
    CPPUNIT_TEST(testEncodingCP852);
    CPPUNIT_TEST(testWrongWhitespaceOption);
    CPPUNIT_TEST(testWrongEncodingOption);
    CPPUNIT_TEST(testWrongCaseOption);
    CPPUNIT_TEST(testWrongTokenNumberingOption);
    CPPUNIT_TEST(testTokenNumberingCONTINUOUS);

    CPPUNIT_TEST_SUITE_END();

public:
    TestCAPI();
    virtual ~TestCAPI();
    void setUp();
    void tearDown();

private:
    void testTwoSimpleInvocations();
    void testWhitespaceKEEP();
    void testWhitespaceAPPEND();
    void testEncodingUTF8();
    void testEncodingISO8859_2();
    void testEncodingCP1250();
    void testEncodingCP852();
    void testWrongWhitespaceOption();
    void testWrongEncodingOption();
    void testWrongCaseOption();
    void testWrongTokenNumberingOption();
    void testTokenNumberingCONTINUOUS();
};

#endif	/* TESTCAPI_HPP */

