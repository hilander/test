#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <poller.hpp>
#include <tr1/memory>
#include <memory>
#include <pthread.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <signal.h>

using namespace std;
using scheduler::poller;
using std::tr1::shared_ptr;

using std::string;

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

int main(int,char**)
{
  shared_ptr< poller > p( poller::get( new ::pthread_mutex_t() ) );

	protoent *pe = getprotobyname( "tcp" );

	sockaddr_in sar;
	sar.sin_family = AF_INET;
	sar.sin_addr.s_addr = INADDR_ANY;
	sar.sin_port = htons( 8100 );

	signal( SIGPIPE, SIG_IGN );

	int sa = socket( AF_INET, SOCK_STREAM, pe->p_proto );
	int orig_flags = fcntl( sa, F_GETFL );
	fcntl( sa, F_SETFL, orig_flags | O_NONBLOCK );

	bind( sa, (sockaddr*)&sar, sizeof(sockaddr_in) );
	listen( sa, 10 );

	int sw = 0;
	sockaddr_in peer_addr;
	socklen_t peer_len = (socklen_t) sizeof(peer_addr);
	while ( sw == 0 )
	{
		sw = accept( sa, (sockaddr*) &peer_addr, &peer_len );
	}

	p->add( sw );
  bool not_read = true;
  do
  {
    auto_ptr< vector < ::epoll_event > > pv( p->poll() );
    if ( pv.get() != 0 )
    {
      std::cout << "poller_server: something on socket occured" << std::endl;
      not_read = false;

      if ( (*pv)[0].data.fd == sa  && (*pv)[0].events & EPOLLIN )
      {
        char* buf = "OK";
        while ( read( sa, buf, 3 ) != 3 )
        {
        }
        std::cout << "poller_server: socket correct, event EPOLLOUT, written: " << buf << std::endl;
      }
    }
  }
  while( not_read )
    ;
  cout << "poller_server: finished" << endl;
  return 0;
}
