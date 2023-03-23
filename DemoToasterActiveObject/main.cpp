#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <mutex>
#include <chrono>
#include <thread>

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

    toaster->start();

    while (true)
    {
        toaster->put_external_entity_event(ExternalEntityEvtType::door_closed);
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::cout << "---------------------------" << std::endl;
        toaster->put_external_entity_event(ExternalEntityEvtType::door_opened);
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::cout << "---------------------------" << std::endl;
    }

    return 0;
}