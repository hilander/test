#include "libcoro_tests.hpp"
#include <coroutine.hpp>
#include <factory.hpp>

namespace libcoro_tests
{
class Ct : public libcoro::coroutine
{
	int tested_value;

	public:

	Ct(libcoro::coroutine::ptr c)
	{
		return_coroutine = c;
	}

	int get_value()
	{
		return tested_value;
	}

	virtual void run()
	{
		tested_value = 1;
		yield ();
		tested_value++;
		yield ();
		tested_value++;
	}
};

class Ct1 : public libcoro::coroutine
{
};

CoroutineTests::CoroutineTests()
	: base_coroutine(libcoro::factory::create_coroutine())
{
}

void
CoroutineTests::setUp()
{
	tested_coroutine = new Ct(base_coroutine);
	another_coroutine = new Ct1();

	libcoro::factory::assign_coroutine(tested_coroutine);
	libcoro::factory::assign_coroutine(another_coroutine);
}

void
CoroutineTests::tearDown()
{
	delete tested_coroutine;
	delete another_coroutine;
}

void
CoroutineTests::testStart()
{

	tested_coroutine->start(base_coroutine);
	CPPUNIT_ASSERT(tested_coroutine->get_value() == 1);
	
	tested_coroutine->start(base_coroutine);
	CPPUNIT_ASSERT(tested_coroutine->get_value() == 2);

	tested_coroutine->rewind();
	tested_coroutine->start(base_coroutine);
	CPPUNIT_ASSERT(tested_coroutine->get_value() == 1);
}

void
CoroutineTests::testGo()
{
	tested_coroutine->start(base_coroutine);
	CPPUNIT_ASSERT(tested_coroutine->get_value() == 1);

	
	tested_coroutine->start(base_coroutine);
	CPPUNIT_ASSERT(tested_coroutine->get_value() == 2);

	tested_coroutine->start(base_coroutine);
	CPPUNIT_ASSERT(tested_coroutine->get_value() == 3);
}

}
