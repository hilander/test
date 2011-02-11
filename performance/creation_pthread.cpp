#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <sys/mman.h>
#include <sched.h>
#include <pthread.h>

#ifndef PTHREAD_STACK_MIN
#define PTHREAD_STACK_MIN 16384
#endif
const int thread_count = 1000;
using namespace std;

void* run (void* x)
{
	for (int i = 0; i < 1000; i++)
	{
		pthread_yield();
	}
	return 0;
}

int main(int, char**)
{
	pthread_attr_t stack_attr;
	pthread_attr_init (&stack_attr);
	pthread_t *ht = new pthread_t[thread_count];

	void **stacks = new void*[PTHREAD_STACK_MIN*thread_count];
	int i;
	for (int k = 0; k < 10; k++)
	{
		for (i = 0; i < thread_count; i++)
		{
			stacks[i] = mmap(0, PTHREAD_STACK_MIN, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		}

		pthread_barrier_t barrier;
		pthread_barrier_init(&barrier, 0, thread_count);

		for (i = 0; i < thread_count; i++)
		{
			//ht[i] = pthread_t();
			int rv = 0;
			//if (stack != 0)
			//{
				pthread_attr_setstack(&stack_attr, stacks[i], PTHREAD_STACK_MIN);
				rv = pthread_create( &ht[i], &stack_attr, &run, (void*)(&barrier) );
			//}
			//else
			//{
				//std::cout << "Error: Couldn't allocate stack\n";
				//rv = errno;
			//}
			if (rv != 0)
			{
				//wywal error
				std::cout << "Error! in thread no: " << i << "\n" << ::strerror(rv) << std::endl;
				for (int j = 0; j < i; j++)
				{
					pthread_cancel(ht[j]);
				}
				::exit(EXIT_FAILURE);
			}
		}

		for (i = 0; i < thread_count; i++)
		{
			pthread_join( ht[i], 0 );
		}

		for (i = 0; i < thread_count; i++)
		{
			munmap(stacks[i], PTHREAD_STACK_MIN);
		}
	}

	delete[] ht;
	return 0;
}

