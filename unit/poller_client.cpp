#include <iostream>
#include <poller.hpp>

#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <errno.h>
#include <pthread.h>
#include <memory>
#include <vector>
#include <unistd.h>
#include <fcntl.h>

using std::string;
using std::auto_ptr;
using std::vector;

void s_err( int num, string& s )
{
  s.clear();

	switch (num)
	{
		case EACCES:
			s = string ( "EACCES" );
			break;

		case EPERM:
			s = string ( "EPERM" );
			break;

		case EADDRINUSE:
			s = string ( "EADDRINUSE" );
			break;

		case EAFNOSUPPORT:
			s = string ( "EAFNOSUPPORT" );
			break;

		case EAGAIN :
			s = string ( "EAGAIN" );
			break;

		case EALREADY:
			s = string ( "EALREADY" );
			break;

		case EBADF :
			s = string ( "EBADF" );
			break;

		case ECONNREFUSED:
			s = string ( "ECONNREFUSED" );
			break;

		case EFAULT:
			s = string ( "EFAULT" );
			break;

		case EINPROGRESS:
			s = string ( "EINPROGRESS" );
			break;

		case EINTR:
			s = string ( "EINTR" );
			break;
		case EISCONN:
			s = string ( "EISCONN" );
			break;

		case ENETUNREACH:
			s = string ( "ENETUNREACH" );
			break;

		case ENOTSOCK:
			s = string ( "ENOTSOCK" );
			break;

		case ETIMEDOUT:
			s = string ( "EACCES" );
			break;
	}
}

const int default_port = 8100;

int main(int,char**)
{
  ::pthread_mutex_t m;
  using scheduler::poller;
  // 0. create poller object
  poller::ptr p( poller::get( &m ) );
  signal( SIGPIPE, SIG_IGN );

  // 1. create client socket
	sockaddr_in sar;
	sar.sin_family = AF_INET;
	inet_pton( AF_INET, "127.0.0.1", &sar.sin_addr );
	sar.sin_port = htons( default_port );

	int sa = socket( AF_INET, SOCK_STREAM, 0 );
  int orig_flags = fcntl( sa, F_GETFD );
  fcntl( sa, F_SETFD, orig_flags | O_NONBLOCK );
  
	int sw = connect( sa, (sockaddr*)&sar, sizeof(sockaddr_in) );

  // 2. add its fd to poller
  if ( sw != 0 )
  {
    string error_name;
    s_err( errno, error_name );
    std::cout << "Error: " << error_name << std::endl;
    return 1;
  }
  else
  {
    if ( ! p->add( sa ) )
    {
      string error_name;
      s_err( errno, error_name );
      std::cout << "Error: " << error_name << std::endl;
    }
  }

  // 3. try to communicate with server
  bool not_read = true;
  do
  {
    auto_ptr< vector < ::epoll_event > > pv( p->poll() );
    if ( pv->size() > 0 )
    {
      std::cout << "poller_client: something on socket occured" << std::endl;
      not_read = false;

      if ( (*pv)[0].data.fd == sa  && (*pv)[0].events & EPOLLOUT )
      {
        char* buf = "OK";
        while ( write( sa, buf, 3 ) != 3 )
        {
        }
        std::cout << "poller_client: socket correct, event EPOLLOUT, written: " << buf << std::endl;
      }
    }
  }
  while( not_read )
    ;

  // 4. finish working

  p->remove( sa );
  close ( sa );
  std::cout << "poller_client: End." << std::endl;
  return 0;
}
