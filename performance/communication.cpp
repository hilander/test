#include <pthread.h>
#include <scheduler_tools.hpp>
#include <iostream>

using namespace std;
using namespace scheduler;

//write: in, read: out
void* t1(void* p)
{
  raw_pipe* rp = static_cast< raw_pipe* >( p );
  spawned_data sd = { END, 0 };
  int i;

  //write... 
  for ( i = 0; i < 10; i++ )
  {
    bool b = false;
    do
    {
      b = rp->write_in( &sd );
    }
    while ( ! b )
      ;
  }

  //read... 
  for ( i = 0; i < 10; i++ )
  {
    bool b = false;
    do
    {
      b = rp->read_out( &sd );
    }
    while ( ! b )
      ;
  }
  return 0;
}

//write: out, read: in
void* t2(void* p)
{
  raw_pipe* rp = static_cast< raw_pipe* >( p );
  spawned_data sd;
  int i;

  for ( i = 0; i < 10; i++ )
  {
    bool b = false;
    do
    {
      b = rp->read_in( &sd );
    }
    while ( ! b )
      ;
  }

  spawned_data sdd = { SPAWN, 0 };
  for ( i = 0; i < 10; i++ )
  {
    bool b = false;
    do
    {
      b = rp->write_out( &sdd );
    }
    while ( ! b )
      ;
  }
  //cout << "t2: OK." << endl;
  return 0;
}

int main(int,char**)
{
  raw_pipe rp;
	pthread_attr_t stack_attr[2];
	pthread_attr_init (&stack_attr[0]);
	pthread_attr_init (&stack_attr[1]);

  pthread_t threads[2];
  int pt1 = pthread_create( &threads[0], &stack_attr[0], &t1, static_cast< raw_pipe* >( &rp ) );
  int pt2 = pthread_create( &threads[1], &stack_attr[1], &t2, static_cast< raw_pipe* >( &rp ) );

  if ( ( pt1 == 0 ) && ( pt2 == 0 ) )
  {
    //pthread_join( threads[0], 0 );
    pthread_join( threads[1], 0 );
    cout << "Communication Test finished (OK, I hope...)." << endl;
  }
  else
  {
    cout.flush();
    return 1;
  }

  cout.flush();
  return 0;
}
