#include <iostream>
#include <poller.hpp>
#include <tr1/memory>
#include <pthread.h>

using namespace std;
using scheduler::poller;
using std::tr1::shared_ptr;

int main(int,char**)
{
  shared_ptr< poller > p( poller::get( new ::pthread_mutex_t() ) );
  cout << "poller_server: finished" << endl;
  return 0;
}
