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

    // boost::bind(&Toaster::callback_external_entity_event, toaster, _1);

    toaster->start();

    while(true);

    return 0;
}