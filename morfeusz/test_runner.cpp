/*
 * File:   test_results_iterator.cpp
 * Author: lennyn
 *
 * Created on Jun 27, 2014, 11:44:49 AM
 */

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include "tests/TestCAPI.hpp"
#include "tests/TestMorfeusz.hpp"
#include "morfeusz2.h"

int main() {
    
    morfeusz::Morfeusz::dictionarySearchPaths.push_front(".");
    morfeusz::Morfeusz::dictionarySearchPaths.push_front("morfeusz");
    
    CPPUNIT_NS::TestRunner runner;
    CPPUNIT_NS::TestResult controller;
    CPPUNIT_NS::TestResultCollector result;
    controller.addListener(&result);
    
    runner.addTest(TestCAPI::suite());
    runner.addTest(TestMorfeusz::suite());
    
    runner.run(controller);
    
    CPPUNIT_NS::CompilerOutputter outputter(&result, CPPUNIT_NS::stdCOut());
    outputter.write();
    return result.wasSuccessful() ? 0 : 1;
}
