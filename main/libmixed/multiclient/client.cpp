#include <iostream>
#include <sstream>
#include <cstdio>

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
    f_client( char* address_c_str, int port )
      : _addr( address_c_str )
			, _port( port )
    {}

    virtual void go()
    {
			int sa = init_socket();

			if ( sa < 0 )
			{
				return;
			}
			
			int n = 300;
      char num[6];
			sprintf( num, "%6d", n );
			string buf( string( "HELLO:" ) + string( num ) );
      ssize_t written_bytes = buf.size();
			char* b = (char*)buf.c_str();
			if ( send_packet( written_bytes, b, sa ) == written_bytes )
			{
				//std::cout << "fiber_client: I wrote " << (int)written_bytes << " bytes." << std::endl;
			}

			char sndbuf[1];
			char recbuf[1];
			for ( int i = 0; i < n; i++ )
			{
				socket_read( recbuf, 1, sa );
				sndbuf[0] = recbuf[0];
				socket_write( sndbuf, 1, sa );
			}
				//std::cout << "fiber_client: I wrote " << (int)n << " bytes." << std::endl;
      do_close( sa );
    }

	private:

		void socket_read( char* buf, ssize_t bytes, int fd )
		{
      while ( ! this->read( buf, bytes, fd ) )
      {
				yield();
      }
		}

		void socket_write( char* buf, ssize_t bytes, int fd )
		{
      while ( ! this->write( buf, bytes, fd ) )
      {
				yield();
      }
		}

		int init_socket()
		{
      sockaddr_in sar;
      sar.sin_family = AF_INET;
      inet_pton( AF_INET, _addr, &sar.sin_addr );
      sar.sin_port = htons( _port );

      int sa = socket( AF_INET, SOCK_STREAM, 0 );
      int orig_flags = fcntl( sa, F_GETFL );
      fcntl( sa, F_SETFL, orig_flags | O_NONBLOCK );

      int sw = 0;
      
      do
      {
				yield();
        sw = ::connect( sa, (const sockaddr*)&sar, sizeof(sar) );
      }
      while ( sw != 0 && ( errno == EINPROGRESS || errno == EALREADY ) )
        ;

      if ( sw != 0 )
      {
        string error_name;
        s_err( errno, error_name );
        std::cout << "poller_client: connect() error: " << error_name << std::endl;
        return -1;
      }
      else
      {
        _supervisor->init_client( sa );
      }

			return sa;
		}

		ssize_t send_packet( ssize_t size, char* buf, int sa )
		{
			ssize_t written_bytes = size;
      while ( ! this->write( buf, written_bytes, sa ) )
      {
				yield();
      }
      if ( written_bytes == size )
      {
        std::cout << "fiber_client: write ok" << std::endl;
      }
			return written_bytes;
      //std::cout << "fiber_client: connect " << ( sa ? "ok." : "fail." ) << std::endl;
		}
		
  private:
    char* _addr;
		int _port;
};

int main(int argc ,char* argv[])
{
	using std::stringstream;

	if ( argc != 3 )
	{
		std::cout << "Error! Improper number of bytes!" << std::endl;
		return 1;
	}

  signal( SIGPIPE, SIG_IGN );
	scheduler::ueber_scheduler us;
	
	us.init();

	stringstream sstr;
	sstr << argv[2] ;
	int port;
	sstr >> port;

  f_client fcl = f_client( argv[1], port );
  us.spawn( &fcl );
  us.join_u_sch();

  return 0;
}
