#include <iostream>
#include <scheduler.hpp>
#include <userspace_scheduler.hpp>
#include <fiber.hpp>

using namespace scheduler;

class first_fiber : public fiber::fiber
{
    public:
    virtual void go()
    {
        std::cout << "first_fiber" << std::endl;
    }
};

class second_fiber : public fiber::fiber
{
    public:
    virtual void go()
    {
        std::cout << "second_fiber" << std::endl;
    }
};

int main(int,char**)
{
    ueber_scheduler us;

    first_fiber ff;
    second_fiber sf;

    std::list< fiber::fiber::ptr > l1, l2;

    l1.push_back( &ff );
    l2.push_back( &sf );

    std::cout << "fibers OK" << std::endl;

    userspace_scheduler u1( &us, l1 ), u2( &us, l2 );

    std::list< userspace_scheduler* >* ul = new std::list< userspace_scheduler* >();

    ul->push_back( &u1 );
    ul->push_back( &u2 );

    std::cout << "userspace_scheduler OK" << std::endl;

    us.init( ul );

	us.join_u_sch();

    std::cout << "OK" << std::endl;
    return 0;
}
