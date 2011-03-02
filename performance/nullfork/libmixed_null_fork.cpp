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
		}
};

int main(int,char**)
{
	int thread_count = 1000;
	scheduler::ueber_scheduler us;
	us.init();

	forked fs[thread_count];
	int i;
	for (i=0; i<thread_count; i++)
	{
	us.spawn(&fs[i]);
	}

	us.join_u_sch();

	return 0;
}
