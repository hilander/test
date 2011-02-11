#include <iostream>
#include <pth.h>

const int thread_count = 1000;

static void* run (void*)
{
	for (int i = 0; i < 1000; i++)
	{
		pth_yield( 0 );
	}
	//std::cout << "." << std::endl;
	return 0;
}

int main(int, char**)
{
	pth_attr_t attr;
	if ( pth_init() == FALSE )
	{
		std::cout << "Init fuckup" << std::endl;
		return 1;
	}
	
	attr = pth_attr_new();
	pth_attr_set(attr, PTH_ATTR_NAME, "ticker");
  pth_attr_set(attr, PTH_ATTR_STACK_SIZE, 64*1024);
  pth_attr_set(attr, PTH_ATTR_JOINABLE, TRUE);


	for (int i=0; i<thread_count; i++)
	{
  pth_t th = pth_spawn(attr, &run, NULL);
	pth_join( th, 0 );
  	//pth_spawn(attr, &run, NULL);
	}

	//std::cout << ".";
	pth_exit( 0 );
	return 0;
}

