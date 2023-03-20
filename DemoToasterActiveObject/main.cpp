#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "ToasterActiveObject.hpp"


void sigint_handler(int sig)
{
    printf("[sigint_handler] Will exit cleanly...\n");
    exit(0);
}

int main(int argc, char **argv)
{
    /* Binds the SIGINT signal to my custom handler */
    signal(SIGINT, sigint_handler);
    auto toaster = std::make_shared<Toaster>();

    int k = 0;
    std::cin >> k;
    while (true)
    {
        std::cout << "------------------" << std::endl;
        tao::InternalEvent event = (tao::InternalEvent)((k % (int) tao::InternalEvent::evt_max));
        std::cout << "Event: " << stringify(event) << std::endl;
        toaster->state_machine_iteration(event);
        std::cout << "------------------" << std::endl;
        std::cin >> k;
    }

    return 0;
}