#include <iostream>

#include "BoostDeadlineTimer.hpp"

//#define EXAMPLE_A
#define EXAMPLE_B

#if defined(EXAMPLE_A)

int main()
{
    {
        DeadlineTimer myTimer{500, []() { std::cout << "Hello" << std::endl; }, true};
        DeadlineTimer myOtherTimer{1000, []() { std::cout << "World" << std::endl; }, true};

        myOtherTimer.start();
        myTimer.start();
        std::this_thread::sleep_for(std::chrono::seconds(2));
        myTimer.stop();
        std::this_thread::sleep_for(std::chrono::seconds(2));
        myTimer.start(200);
        std::this_thread::sleep_for(std::chrono::seconds(2));
        myTimer.stop();
        myOtherTimer.stop();
    }

    std::string holdon;
    std::cin >> holdon;
    return 0;
}

#elif defined(EXAMPLE_B)

class Entity
{
   public:
    Entity() : m_timer{200, boost::bind(&Entity::callback, this), true}
    {
        m_timer.start();
    }

    void callback()
    {
        std::cout << "Entity::callback()" << std::endl;
    }

   private:
    DeadlineTimer m_timer;
};

int main()
{
    Entity foo{};

    std::this_thread::sleep_for(std::chrono::seconds(2));

    return 0;
}

#endif
