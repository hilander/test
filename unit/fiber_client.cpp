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

#include <fiber.hpp>
#include <scheduler.hpp>
#include <scheduler_tools.hpp>

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

class f_client : public fiber::fiber
{
  public:
    f_client( char* address_c_str )
      : _addr( address_c_str )
    {}

    virtual void go()
    {
      sockaddr_in sar;
      sar.sin_family = AF_INET;
      inet_pton( AF_INET, _addr, &sar.sin_addr );
      sar.sin_port = htons( default_port );

      int sa = socket( AF_INET, SOCK_STREAM, 0 );
      int orig_flags = fcntl( sa, F_GETFL );
      fcntl( sa, F_SETFL, orig_flags | O_NONBLOCK );

      int sw = 0;
      
      do
      {
        //sw = this->connect( sa, d );
        sw = ::connect( sa, (const sockaddr*)&sar, sizeof(sar) );
      }
      while ( sw != 0 && ( errno == EINPROGRESS || errno == EALREADY ) )
        ;

      if ( sw != 0 )
      {
        string error_name;
        s_err( errno, error_name );
        std::cout << "poller_client: connect() error: " << error_name << std::endl;
        return;
      }
      else
      {
        _supervisor->init_client( sa );
      }

      char buf[3] = "OK";
      ssize_t written_bytes = 3;

      std::cout << "fiber_client: write..." << std::endl;
      while ( ! this->write( buf, written_bytes, sa ) )
      {
      }
      if ( written_bytes == 3 )
      {
        std::cout << "fiber_client: write ok" << std::endl;
      }
      //std::cout << "fiber_client: connect " << ( sa ? "ok." : "fail." ) << std::endl;
      do_close( sa );
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

  f_client fcl = f_client( ( argc == 2 ) ? argv[1] : loopback );
  us.spawn( &fcl );
  us.join_u_sch();

  return 0;
}
