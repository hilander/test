#include <coroutine.hpp>
#include <factory.hpp>
#include "libcoro_tests.hpp"
#include <exception>


namespace libcoro_tests
{

struct Exc
{
	int z;
	Exc(int i) : z(i) {}
};

struct E1 : public libcoro::coroutine
{
	void run()
	{
		throw new Exc(0);
	}
};

struct E2 : public libcoro::coroutine
{
	void run()
	{
		throw std::exception();
	}
};

ExceptionTests::ExceptionTests()
	: base_coroutine(libcoro::factory::create_coroutine())
{
}

void
ExceptionTests::setUp()
{
	exc = new E1();
	exc_std = new E2();
	//libcoro::factory cf = libcoro::factory();
	libcoro::factory::assign_coroutine(exc);
	libcoro::factory::assign_coroutine(exc_std);
}

void
ExceptionTests::tearDown()
{
	delete exc;
	delete exc_std;
}

void
ExceptionTests::testThrow()
{
	/* Brak stanów w koprocedurze -- brak testu dla stanów.
	 exc->start(base_coroutine);
	 CPPUNIT_ASSERT(exc->state.get() == libcoro::state_controller::ERROR);
	
		exc_std->start(base_coroutine);
		CPPUNIT_ASSERT(exc_std->state.get() == libcoro::state_controller::EXCEPTION);
	*/
}
	
}
