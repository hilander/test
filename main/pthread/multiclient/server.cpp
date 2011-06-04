#include <iostream>
#include <list>
#include <sstream>
#include <algorithm>

#include <signal.h>
#include <netdb.h>
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

const int default_port = 8100;

void s_err( int num, string& s );

class abs_listener
{
  public:
    virtual void go() = 0;
};

static void* unified_starter( void* p )
{
  abs_listener* obj = static_cast< abs_listener* >( p );
  obj->go();
  return 0;
}

class f_listener : public abs_listener
{
	public:
		f_listener( int fd_ )
		: fd ( fd_ )
		{}

		virtual void go()
		{
      char buf[12];
      ssize_t read_bytes = 12;

      ::read( fd, buf, read_bytes );
			string s = string(buf).substr(6,12);
			std::stringstream sstr;
			sstr << string(buf).substr(6,12);
			int bytes = 0;
			sstr >> bytes;

			char sndbuf[1];
			char recbuf[1];
			sndbuf[0] = 42;
			for ( int i = 0; i < bytes; i++ )
			{
				socket_write( sndbuf, 1 );
				socket_read( recbuf, 1 );
				if ( sndbuf[0] != recbuf[0] )
				{
					std::cout << "Server listener: Client response incorrect." << std::endl;
					break;
				}
			}
			std::cout << "Server listener: end." << std::endl;

      ::close( fd );
		}

	private:
		void socket_read( char* buf, ssize_t bytes )
		{
      // use ::read()
		}

		void socket_write( char* buf, ssize_t bytes )
		{
      // use ::write()
		}

	private:
		int fd;
};

class f_client : public abs_listener
{
  public:
    f_client( char* address_c_str )
      : _addr( address_c_str )
    {}

    virtual void go()
    {

			int max_opened = 100;
			int sa = init_socket();
			if ( sa < 0 )
			{
				return;
			}

			int opened_sockets;
			for ( opened_sockets = 0; opened_sockets < max_opened;  )
			{
				int sw = wait_for_connection( sa );

				if ( sw <= 0 )
				{
					string error_name;
					s_err( errno, error_name );
					std::cout << "poller_client: connect() error: " << error_name << std::endl;
					//return;
				}
				else
				{
					create_listener( sw );
					opened_sockets++;
				}
			}

			::close( sa );;
			std::cout << "Server: exiting. " << std::endl;
    }

	private:

		void create_listener( int listen_descriptor )
		{
			f_listener* l = new f_listener( listen_descriptor );
      ::pthread_t* tp = new ::pthread_t();
      ::pthread_attr_t attrs;
      pthread_create( tp, &attrs, unified_starter, static_cast< void* >( l ) );
			listeners.push_back( tp );
		}

		int init_socket()
		{
      ::protoent *pe = getprotobyname( "tcp" );

      sockaddr_in sar;
      sar.sin_family = AF_INET;
      sar.sin_addr.s_addr = INADDR_ANY;
      sar.sin_port = htons( default_port );

      int sa = socket( AF_INET, SOCK_STREAM, pe->p_proto );
      int orig_flags = fcntl( sa, F_GETFL );
      fcntl( sa, F_SETFL, orig_flags | O_NONBLOCK );

      if ( bind( sa, (sockaddr*)&sar, sizeof(sockaddr_in) ) != 0 )
      {
        string error_name;
        s_err( errno, error_name );
        std::cout << "fiber_server: bind() error: " << error_name << std::endl;
        return -1;
      }

      if ( listen( sa, 10 ) != 0 )
      {
        string error_name;
        s_err( errno, error_name );
        std::cout << "fiber_server: bind() error: " << error_name << std::endl;
        return -1;
      }
			return sa;
		}

		int wait_for_connection( int sa )
		{
      ::sockaddr_in sd;
      ::socklen_t sl = sizeof( ::sockaddr_in );
      return ::connect( sa, ( ::sockaddr* )&sd, sl );
		}

  private:
    char* _addr;
		std::list< ::pthread_t* > listeners;
};

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

int main(int argc ,char* argv[])
{
  signal( SIGPIPE, SIG_IGN );
  char loopback[] = "127.0.0.1";

  f_client fcl = f_client( ( argc == 2 ) ? argv[1] : loopback );
  ::pthread_t* main_thread = new ::pthread_t();
  ::pthread_attr_t attrs;

  return 0;
}

