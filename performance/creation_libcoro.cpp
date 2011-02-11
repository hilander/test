#include <iostream>
#include <coroutine.hpp>
#include <factory.hpp>

const int thread_count = 1000;
class easy : public libcoro::coroutine
{
	public:
	virtual void run (void*)
	{
	}
};

int main(int, char**)
{
	easy ca[thread_count];
	for (int i=0; i<thread_count; i++)
	{
		//ca[i] = new easy();
		libcoro::factory::assign_coroutine(&ca[i]);
		std::cout << "start!" << std::endl;
		ca[i].start(0);
		std::cout << "stop!" << std::endl;
	}
	//delete[] ca;->
}

