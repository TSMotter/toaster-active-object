#include <iostream>

#include "BoostDeadlineTimer.hpp"

int main()
{
    {
        DeadlineTimer myTimer{500, []() {}, true};

        myTimer.start();
        std::this_thread::sleep_for(std::chrono::seconds(2));
        myTimer.stop();
        std::this_thread::sleep_for(std::chrono::seconds(2));
        myTimer.start(200);
        std::this_thread::sleep_for(std::chrono::seconds(2));
        myTimer.stop();
    }

    std::string holdon;
    std::cin >> holdon;
    return 0;
}