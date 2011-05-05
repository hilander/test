#include <iostream>

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
#include <netdb.h>

#include <fiber.hpp>
#include <scheduler.hpp>

using std::string;
using std::auto_ptr;
using std::vector;

const int default_port = 8100;

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

class f_server : public fiber::fiber
{
  public:
    f_server( char* address_c_str )
      : _addr( address_c_str )
    {}

    virtual void go()
    {
      //::pthread_mutex_t m;
      //using scheduler::poller;
      // 0. create poller object
      //poller::ptr p( poller::get( &m ) );

      // 1. create client socket

      protoent *pe = getprotobyname( "tcp" );

      sockaddr_in sar;
      sar.sin_family = AF_INET;
      sar.sin_addr.s_addr = INADDR_ANY;
      //inet_pton( AF_INET, _addr, &sar.sin_addr );
      sar.sin_port = htons( default_port );

      int sa = socket( AF_INET, SOCK_STREAM, pe->p_proto );
      int orig_flags = fcntl( sa, F_GETFL );
      fcntl( sa, F_SETFL, orig_flags | O_NONBLOCK );

      if ( bind( sa, (sockaddr*)&sar, sizeof(sockaddr_in) ) != 0 )
      {
        string error_name;
        s_err( errno, error_name );
        std::cout << "poller_server: bind() error: " << error_name << std::endl;
        return;
      }

      if ( listen( sa, 10 ) != 0 )
      {
        string error_name;
        s_err( errno, error_name );
        std::cout << "poller_server: bind() error: " << error_name << std::endl;
        return;
      }
      bool sw;
      sockaddr_in sa_in;
      sockaddr* ss = (sockaddr*)&sa_in;
      scheduler::accept_connect_data d;
      d.fd = sa;
      d.saddr = (const ::sockaddr&)sa_in;
      do
      {
        sw = this->accept( sa, d );
      }
      while ( !sw  )
        ;
      _supervisor->init_server( sw );

      std::cout << "fiber_server: accept " << ( sw ? "ok." : "fail." ) << std::endl;
      //do_close( sa );
      do_close( sw );
    }

  private:
    char* _addr;
};

int main(int argc ,char* argv[])
{
  signal( SIGPIPE, SIG_IGN );
  char loopback[] = "127.0.0.1";
	scheduler::ueber_scheduler us;
	us.init();

  f_server fcl = f_server( ( argc == 2 ) ? argv[1] : loopback );
  us.spawn( &fcl );
  us.join_u_sch();

  /*
  ::pthread_mutex_t m;
  using scheduler::poller;
  // 0. create poller object
  poller::ptr p( poller::get( &m ) );
  signal( SIGPIPE, SIG_IGN );

  // 1. create client socket
	sockaddr_in sar;
	sar.sin_family = AF_INET;
	if ( argc == 2 )
	{
		inet_pton( AF_INET, argv[1], &sar.sin_addr );
	}
	else
	{
		inet_pton( AF_INET, "127.0.0.1", &sar.sin_addr );
	}
	sar.sin_port = htons( default_port );

	int sa = socket( AF_INET, SOCK_STREAM, 0 );
  int orig_flags = fcntl( sa, F_GETFL );
  fcntl( sa, F_SETFL, orig_flags | O_NONBLOCK );
  
	int sw = 0;
	do
	{
		sw = connect( sa, (sockaddr*)&sar, sizeof(sockaddr_in) );
	}
	while ( sw != 0 && ( errno == EINPROGRESS || errno == EALREADY ) )
		;

  // 2. add its fd to poller
  if ( sw != 0 )
  {
    string error_name;
    s_err( errno, error_name );
    std::cout << "poller_client: connect() error: " << error_name << std::endl;
    return 1;
  }
  else
  {
    if ( ! p->add( sa ) )
    {
      string error_name;
      s_err( errno, error_name );
      std::cout << "poller_client: poller.add() error: " << error_name << std::endl;
    }
  }

  // 3. try to communicate with server
  bool not_read = true;
  do
  {
    auto_ptr< vector < ::epoll_event > > pv( p->poll() );
    if ( pv.get() != 0 )
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
  */
  return 0;
}
