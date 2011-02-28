#include <iostream>
#include <scheduler.hpp>
#include <userspace_scheduler.hpp>
#include <fiber.hpp>

using namespace scheduler;

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
			//std::cout << "Fiber: OK." << std::endl;
			ended = true;
		}
};

int main(int,char**)
{
	int thread_count = 1000;
	scheduler::ueber_scheduler us;
	us.init();
  /*
	std::cout 
		<< "Thread count: " << thread_count
		<< std::endl
		<< "Init: OK."
		<< std::endl;
  */

	forked fs[thread_count];
	int i;
	for (i=0; i<thread_count; i++)
	{
	us.spawn(&fs[i]);
	}
  /*
	std::cout << "Spawn: OK." << std::endl;
  */

	us.join_u_sch();
  /*
	std::cout << "End: OK." << std::endl;
  */
	return 0;
}
