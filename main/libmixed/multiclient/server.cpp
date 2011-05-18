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

void waitfor( fiber::fiber::ptr f )
{
	if ( f != 0 )
	{
		f->wait();
	}
}

struct fl_starter_pack
{
	typedef fl_starter_pack* ptr;
	int fd;
	fiber::fiber::ptr parent;
};

class f_listener : public fiber::fiber
{
	public:
		f_listener( void* data )
		{
			fl_starter_pack::ptr pack = static_cast< fl_starter_pack::ptr >( data );
			fd = pack->fd;
			parent = pack->parent;
		}

		f_listener( int fd_, fiber::fiber::ptr parent_ )
		: fd ( fd_ )
		, parent( parent_ )
		{}

		virtual void go()
		{
			_supervisor->init_server( fd );

      char buf[12];
      ssize_t read_bytes = 12;

      while ( ! this->read( buf, read_bytes, fd ) )
      {
				yield();
      }
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
			scheduler::spawned_data message;
			message.d = scheduler::FIBER_SPECIFIC;
			message.p = 0;
			message.sender = this;
			message.receiver = parent;
			while ( !send( message ) )
			{
				yield();
			}
			while ( !receive( message ) )
			{
				yield();
			}
			std::cout << "Server listener: end." << std::endl;

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
		fiber::fiber::ptr parent;
};

static fiber::fiber::ptr get_listener( void* p )
{
	return new f_listener( p );
}

class f_client : public fiber::fiber
{
  public:
    f_client( char* address_c_str )
      : _addr( address_c_str )
    {}

    virtual void go()
    {

			int max_opened = 20;
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
				else if ( std::find( connections.begin(), connections.end(), sw ) == connections.end() )
				{
					create_listener( sw );
					opened_sockets++;
				}
			}

			//std::cout << "opened_sockets (out of loop): " << opened_sockets << std::endl;
			//std::for_each ( listeners.begin(), listeners.end(), &waitfor );
			wait_for_listeners( max_opened );
			_supervisor->close( sa );;
      //::close( sa );
			std::cout << "Server: exiting. " << std::endl;
    }

	private:
		void wait_for_listeners( int how_many )
		{
			scheduler::spawned_data message;
			int responses = 0;

			while ( responses < how_many )
			{
				while ( !receive( message ) )
				{
					yield();
				}
				responses++;
			}
			for ( int listeners = 0; listeners < how_many; listeners++ )
			{
				message.d = scheduler::FIBER_SPECIFIC;
				message.p = 0;
				message.receiver = message.sender;
				message.sender = this;
				while ( !send( message ) )
				{
					yield();
				}
			}
		}

		void create_listener( int listen_descriptor )
		{
			yield();
			f_listener* l = new f_listener( listen_descriptor, this );
			spawn( l );
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
			_supervisor->init_server( sa );
			return sa;
		}

		int wait_for_connection( int sa )
		{
			int sw = 0;

			scheduler::accept_connect_data data;
			data.fd = sa;
      while ( !this->accept( sa, data ) )
			{
				yield();
        ;
			}
			std::cout << "sa: " << sa << "sw: " << data.fd << std::endl;
			return data.fd;
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

