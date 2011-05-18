#include <iostream>
#include <scheduler.hpp>
#include <userspace_scheduler.hpp>
#include <fiber.hpp>

using namespace scheduler;

class fu : public fiber::fiber
{
	public:
		virtual void go()
		{
			yield();
			std::cout << "fu: OK." << std::endl;
		}
};

class forked : public fiber::fiber
{
	public:
		bool ended;
		forked()
		{
			ended = false;
		}

		virtual void go()
		{
			for (int i = 0; i < 1000; i++)
			{
				yield();
			}
			fiber::fiber::ptr fp = new fu();
			_supervisor->spawn( fp );
			std::cout << "forked: OK." << std::endl;
			ended = true;
		}
};

int main(int,char**)
{
	int thread_count = 10;
	scheduler::ueber_scheduler us;
	us.init();
	forked fs;
	us.spawn( &fs );
  /*
	std::cout 
		<< "Thread count: " << thread_count
		<< std::endl
		<< "Init: OK."
		<< std::endl;

	forked fs[thread_count];
	int i;
	for (i=0; i<thread_count; i++)
	{
	us.spawn(&fs[i]);
	}
  */
	std::cout << "Spawn: OK." << std::endl;
  

	us.join_u_sch();
	std::cout << "End: OK." << std::endl;
	return 0;
}
