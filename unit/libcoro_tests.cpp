#include <coroutine.hpp>
#include <cppunit/TextTestRunner.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/HelperMacros.h>
#include <iostream>
#include "libcoro_tests.hpp"

#define test_call( test_t , method_name) \
(new CppUnit::TestCaller< test_t > ( "\""#method_name"\"" , &test_t::method_name ))

int main(int /*argc*/, char** /*argv*/)
{
	using namespace libcoro_tests;
	CppUnit::TestSuite * ts ( new CppUnit::TestSuite());
	ts->addTest(test_call(CoroutineTests, testStart));
	ts->addTest(test_call(CoroutineTests, testGo));
	ts->addTest(test_call(ExceptionTests, testThrow));

	CppUnit::TextTestRunner runner;
	runner.setOutputter(new CppUnit::CompilerOutputter(&runner.result(), std::cerr));
  runner.addTest( (CppUnit::Test*)ts );

	runner.run();

	return 0;
}
