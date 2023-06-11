#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>

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
        int k = 0;
        std::cin >> k;
        std::cout << "---------------------------" << std::endl;
        toaster->put_external_entity_event(static_cast<ExternalEntityEvtType>(k));
        std::cout << "---------------------------" << std::endl;
    }

    return 0;
}