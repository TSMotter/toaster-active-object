#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "ActiveObject.hpp"

void sigint_handler(int sig)
{
    printf("[sigint_handler] Will exit cleanly...\n");
    exit(0);
}

int main(int argc, char **argv)
{
    /* Binds the SIGINT signal to my custom handler */
    signal(SIGINT, sigint_handler);
    auto entity = std::make_shared<Entity>(1.0, 2.0);

    int k = 0;
    std::cout << "\nExample started. Input a command: " << std::endl;
    std::cin >> k;
    while (true)
    {
        std::cout << "------------------" << std::endl;
        ao::InternalEvent event = (ao::InternalEvent)((k % 3) + 1);
        std::cout << "Event: " << (int) event << std::endl;
        entity->state_machine_iteration(event);
        std::cout << "------------------" << std::endl;
        std::cin >> k;
    }

    return 0;
}