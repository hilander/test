#include <iostream>
#include <scheduler.hpp>
#include <userspace_scheduler.hpp>
#include <scheduler_tools.hpp>
#include <fiber.hpp>
#include <tr1/memory>

using namespace scheduler;

class fiber_data
{
  public:
    int contents;
};

class first_fiber : public fiber::fiber
{
    public:
    virtual void go()
    {
        fiber_data fd;
        fd.contents = 42;

        spawned_data* packet = new spawned_data();
        packet->d = FIBER_SPECIFIC;
        packet->p = &fd;
        packet->sender = this;
        packet->receiver = receiver;

        //std::cout << "first_fiber: packet prepared." << std::endl;

        while ( ! send( packet ) )
        {
          yield();
        }
				//std::cout << "first_fiber: packet sent." << std::endl;
				/*
        else
        {
          std::cout << "first_fiber: packet _NOT_ sent." << std::endl;
        }
				*/

        while ( ! receive( packet ) )
        {
          // czekamy na pakiet, możemy oddać czas innym włóknom:
          yield();
        }

        //std::cout << "first_fiber end;" << std::endl;
    }

    void set_receiver( fiber::fiber::ptr receiver_ )
    {
        receiver = receiver_;
    }

    private:
    fiber::fiber::ptr receiver;
};

class second_fiber : public fiber::fiber
{
    public:
    virtual void go()
    {
        std::tr1::shared_ptr< spawned_data > packet2 = std::tr1::shared_ptr< spawned_data >( new spawned_data() );
        spawned_data* packet = packet2.get();

        //std::cout << "second_fiber: start." << std::endl;

        while ( ! receive( packet ) )
        {
          yield();
        }

        //std::cout << "second_fiber: packet received." << std::endl;

        packet->d = FIBER_SPECIFIC;
        packet->p = 0;
        packet->sender = this;
        packet->receiver = receiver;

        while ( ! send( packet ) )
        {
          yield();
        }
          //std::cout << "second_fiber: packet sent." << std::endl;
					/*
        else
        {
          std::cout << "second_fiber: packet _NOT_ sent." << std::endl;
        }
				*/

        //std::cout << "second_fiber: All done OK." << std::endl;
    }

    void set_receiver( fiber::fiber::ptr receiver_ )
    {
        receiver = receiver_;
    }

    private:
    fiber::fiber::ptr receiver;
};

void communicate_using_two_schedulers()
{
    ueber_scheduler us;

    first_fiber ff;
    second_fiber sf;

    ff.set_receiver( &sf );
    sf.set_receiver( &ff );

    std::list< fiber::fiber::ptr > l1, l2;

    l1.push_back( &ff );
    l2.push_back( &sf );

    //std::cout << "fibers OK" << std::endl;

    userspace_scheduler u1( &us, l1 ), u2( &us, l2 );
    
    ff.set_supervisor( &u1 );
    sf.set_supervisor( &u2 );

    std::list< userspace_scheduler* >* ul = new std::list< userspace_scheduler* >();

    ul->push_back( &u1 );
    ul->push_back( &u2 );

    //std::cout << "userspace_scheduler OK" << std::endl;

    us.init( ul );

	us.join_u_sch();

	//std::cout << "Test for two schedulers OK" << std::endl;
}

void communicate_using_one_scheduler()
{
    ueber_scheduler us;

    first_fiber ff;
    second_fiber sf;

    ff.set_receiver( &sf );
    sf.set_receiver( &ff );

    std::list< fiber::fiber::ptr > l;

    l.push_back( &ff );
    l.push_back( &sf );

    userspace_scheduler u( &us, l );
    
    ff.set_supervisor( &u );
    sf.set_supervisor( &u );

    std::list< userspace_scheduler* >* ul = new std::list< userspace_scheduler* >();

    ul->push_back( &u );


    us.init( ul );

	us.join_u_sch();

    //delete ul;
    //std::cout << "Test for one scheduler OK" << std::endl;
}

int main(int,char**)
{
    communicate_using_one_scheduler();
    //std::cout << "OK" << std::endl;

    communicate_using_two_schedulers();

    //std::cout << "OK" << std::endl;
    return 0;
}
