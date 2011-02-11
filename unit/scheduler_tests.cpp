#include <coroutine.hpp>
#include <cppunit/TextTestRunner.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/HelperMacros.h>
#include <iostream>
#include "scheduler_tests.hpp"

#define test_call( test_t , method_name) \
(new CppUnit::TestCaller< test_t > ( "\""#method_name"\"" , &test_t::method_name ))

int main(int /*argc*/, char** /*argv*/)
{
	using namespace scheduler_tests;
	CppUnit::TestSuite * ts ( new CppUnit::TestSuite());
	ts->addTest(test_call(SchedulerTests, testSpawn));
	ts->addTest(test_call(SchedulerTests, testCommunication));
	ts->addTest(test_call(SchedulerTests, testWatcher));
	ts->addTest(test_call(SchedulerTests, testYield));
	ts->addTest(test_call(SchedulerTests, testStartUeberScheduler));
	ts->addTest(test_call(SchedulerTests, testJoinUeberScheduler));

	CppUnit::TextTestRunner runner;
	runner.setOutputter(new CppUnit::CompilerOutputter(&runner.result(), std::cerr));
  runner.addTest( (CppUnit::Test*)ts );

	runner.run();

	return 0;
}
