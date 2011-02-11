#ifndef __LIBCORO_TESTS__
#define __LIBCORO_TESTS__

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <factory.hpp>
#include "../../scheduler/scheduler.hpp"
#include "../../scheduler/userspace_scheduler.hpp"
#include "../../fiber/fiber.hpp"
#include "../../fiber/resource.hpp"
#include <iostream>

using std::auto_ptr;

/**
 * \brief Przestrzeń nazw dla testów jednostkowych.
 */
namespace scheduler_tests
{

class thread : public fiber::fiber
{
	private:
		int _my_number;

	public:
	thread(int i)
	{
		_my_number = i;
	}
	thread() : _my_number( 0 )
	{}

	virtual void run() 
	{ 
		//using namespace fiber;
		//message m = message( message::EMPTY, 0 );
		//send( m, 0 );
	}
};

class yielingThread : public fiber::fiber
{
	private:
		int _my_number;

	public:
	yielingThread(int i)
	{
		_my_number = i;
	}

	virtual void run() 
	{ 
		yield();
	}
};

/** Testy dla planisty.
 * \class SchedulerTests Testy dla planisty
 */
class SchedulerTests : public CppUnit::TestFixture
{
	public:

	scheduler::userspace_scheduler* s;
	scheduler::ueber_scheduler us;

	SchedulerTests() {  } 

	/** Inicjalizacja zasobów dla pojedynczego testu.
	 * metoda setUp jest wywoływana przed
	 * wykonaniem każdego testu.
	 */
	void setUp() 
	{ 
		s = new scheduler::userspace_scheduler(0);
	} 

	void tearDown() 
	{
		delete s;
	}

	void testSpawn() 
	{
		int N = 100000;
		thread* t[N];
		s->init();

		for (int k = 0; k < N; k++)
		{
			t[k] = new thread(k);
			s->spawn(t[k]);
		}

		//CPPUNIT_ASSERT( s->get_workload() == N );

		//s->start();
		s->join();

		std::cout << "testSpawn: " << s->get_workload() << std::endl;
		CPPUNIT_ASSERT( s->get_workload() == 0 );

	}

	void testCommunication()
	{
		thread t;
		fiber::message m( fiber::message::EMPTY, 0 );
		bool s = t.send( m, 0 );
		CPPUNIT_ASSERT( s == true );

		fiber::message rm = t.receive();
		CPPUNIT_ASSERT( rm.kind == fiber::message::NO_MESSAGE );
	}
	void testWatcher()
	{
		s->init();
		thread t;
		fiber::resource r;
		t.get_resources().push_back( &r );
		CPPUNIT_ASSERT( ( (*(t.get_resources().begin()))->get_watcher() == 0) );
		s->spawn( &t );
		s->join();
		CPPUNIT_ASSERT( ( (*(t.get_resources().begin()))->get_watcher() != 0) );
		CPPUNIT_ASSERT( ( (*(t.get_resources().begin())) == &r) );
	}
	void testYield()
	{
		s->init();
		yielingThread t1( 1 ), t2( 2 );
		s->spawn( &t1 );
		s->spawn( &t2 );

		s->start();
		s->join();
		CPPUNIT_ASSERT( s->empty() );
	}

	void testStartUeberScheduler()
	{
		//std::cout << "testStartUeberScheduler" << std::endl;
		//us.init();
		//us.start();
		//us.join_u_sch();
	}

	void testJoinUeberScheduler()
	{
		//std::cout << "testJoinUeberScheduler" << std::endl;
		//us.init();
		//us.join_u_sch();
	}
};

}
#endif
