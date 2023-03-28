#include <iostream>

#include "BoostDeadlineTimer.hpp"

int main()
{
    DeadlineTimer myTimer{500, []() { std::cout << "Callbacked" << std::endl; }, true};

    std::this_thread::sleep_for(std::chrono::seconds(2));
    myTimer.start();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    myTimer.stop();


    return 0;
}