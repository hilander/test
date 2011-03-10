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

        std::cout << "first_fiber: packet prepared." << std::endl;

        if ( send( packet ) )
        {
          std::cout << "first_fiber: packet sent." << std::endl;
        }
        else
        {
          std::cout << "first_fiber: packet _NOT_ sent." << std::endl;
        }

        while ( ! receive( packet ) )
        {
          // czekamy na pakiet, możemy oddać czas innym włóknom:
          yield();
        }

        std::cout << "first_fiber end;" << std::endl;
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
        spawned_data* packet = new spawned_data();
        spawned_data* packet2;

        while ( ! receive( packet ) )
        {
          yield();
        }

        std::cout << "second_fiber: packet received." << std::endl;

        packet->d = FIBER_SPECIFIC;
        packet->p = 0;
        packet->sender = this;
        packet->receiver = receiver;

        if ( send( packet ) )
        {
          std::cout << "second_fiber: packet sent." << std::endl;
        }
        else
        {
          std::cout << "second_fiber: packet _NOT_ sent." << std::endl;
        }

        std::cout << "second_fiber: All done OK." << std::endl;
    }

    void set_receiver( fiber::fiber::ptr receiver_ )
    {
        receiver = receiver_;
    }

    private:
    fiber::fiber::ptr receiver;
};

int main(int,char**)
{
    ueber_scheduler us;

    first_fiber ff;
    second_fiber sf;

    ff.set_receiver( &sf );
    sf.set_receiver( &ff );

    std::list< fiber::fiber::ptr > l1, l2;

    l1.push_back( &ff );
    l2.push_back( &sf );

    std::cout << "fibers OK" << std::endl;

    userspace_scheduler u1( &us, l1 ), u2( &us, l2 );
    
    ff.set_supervisor( &u1 );
    sf.set_supervisor( &u2 );

    std::list< userspace_scheduler* >* ul = new std::list< userspace_scheduler* >();

    ul->push_back( &u1 );
    ul->push_back( &u2 );

    std::cout << "userspace_scheduler OK" << std::endl;

    us.init( ul );

	us.join_u_sch();

    std::cout << "OK" << std::endl;
    return 0;
}
