#ifndef __LIBCORO_TESTS__
#define __LIBCORO_TESTS__

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <coroutine.hpp>

using std::auto_ptr;

/**
 * \brief Przestrzeń nazw dla testów jednostkowych.
 * Przestrzeń <tt> libcoro_tests </tt> zawiera zbiór
 * testów jednostkowych (<i>Unit Tests</i>).
 * \see <a href="http://en.wikipedia.org/wiki/Unit_testing">Wikipedia (en)</a>
 */
namespace libcoro_tests
{

struct E1;
struct E2;

class ExceptionTests : public CppUnit::TestFixture
{
	E1* exc;
	E2* exc_std;
	libcoro::coroutine * base_coroutine;

	public:

	ExceptionTests();

	void setUp();
	void tearDown();


	void testThrow();
};

class Ct;
class Ct1;

/** Testy dla koprocedur.
 * \class CoroutineTests Testy dla koprocedur
 */
class CoroutineTests : public CppUnit::TestFixture
{
	libcoro::coroutine * base_coroutine;
	Ct * tested_coroutine;
	Ct1 * another_coroutine;

	public:

	CoroutineTests();

	/** Inicjalizacja zasobów dla pojedynczego testu.
	 * metoda setUp jest wywoływana przed
	 * wykonaniem każdego testu.
	 */
	void setUp();
	void tearDown();


	void testStart();
	void testGo();
};

}
#endif
