#include <iostream>
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

#include <fiber.hpp>
#include <scheduler.hpp>
#include <scheduler_tools.hpp>

using std::string;
using std::auto_ptr;
using std::vector;

const int default_port = 8100;

void s_err( int num, string& s );

bool is_finished( fiber::fiber::ptr f )
{
	bool rv = f->state.get() == libcoro::state_controller::FINISHED;
	if ( rv )
	{
		std::cout << "listener finished working." << std::endl;
	}
	return rv;
}

class f_listener : public fiber::fiber
{
	public:
		f_listener( int fd_ )
		: fd ( fd_ )
		{}

		virtual void go()
		{
			_supervisor->init_server( fd );

      char buf[12];
      ssize_t read_bytes = 12;

      //std::cout << "fiber_server: read..." << std::endl;
      while ( ! this->read( buf, read_bytes, fd ) )
      {
				yield();
      }
			string s = string(buf).substr(6,12);
			std::stringstream sstr;
			sstr << string(buf).substr(6,12);
			int bytes = 0;
			sstr >> bytes;
			//std::cout << "fiber_server: " << bytes << " bytes to read" << std::endl;

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
      do_close( fd );
		}

	private:
		void socket_read( char* buf, ssize_t bytes )
		{
      while ( ! this->read( buf, bytes, fd ) )
      {
				yield();
      }
		}

		void socket_write( char* buf, ssize_t bytes )
		{
      while ( ! this->write( buf, bytes, fd ) )
      {
				yield();
      }
		}

	private:
		int fd;
};

class f_client : public fiber::fiber
{
  public:
    f_client( char* address_c_str )
      : _addr( address_c_str )
    {}

    virtual void go()
    {

			int sa = init_socket();
			if ( sa < 0 )
			{
				return;
			}

			for ( int opened_sockets = 0; opened_sockets < 2; opened_sockets++ )
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
				}
			}

			int s = listeners.size();
			do
			{
				std::remove_if( listeners.begin(), listeners.end(), is_finished );
			}
			while ( listeners.size() > 0  )
				;
			::shutdown( sa, SHUT_RDWR );
      //::close( sa );
			//std::cout << "Server: exiting. " << s << std::endl;
    }

	private:
		void create_listener( int listen_descriptor )
		{
			f_listener* l = new f_listener( listen_descriptor );
			create_fiber( l );
			listeners.push_back( l );
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
			int sw;

      ::sockaddr_in sw_in;
      ::socklen_t sw_len = (::socklen_t) sizeof(::sockaddr_in);
      do
      {
        sw = ::accept( sa, (sockaddr*) &sw_in, &sw_len );
				yield();
      }
      while ( sw <= 0  && errno == EAGAIN )
        ;
			return sw;
		}

  private:
    char* _addr;
		std::list< fiber::fiber::ptr > listeners;
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
	scheduler::ueber_scheduler us;
	us.init();

  f_client fcl = f_client( ( argc == 2 ) ? argv[1] : loopback );
  us.spawn( &fcl );
  us.join_u_sch();

  return 0;
}

