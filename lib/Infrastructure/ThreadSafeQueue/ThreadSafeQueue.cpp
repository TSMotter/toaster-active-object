#include <iostream>
#include <memory>
#include <thread>

#include "ThreadSafeQueue.hpp"

std::shared_ptr<spdlog::logger> myLogger = spdlog::stdout_color_mt("ThreadSafeQueue");
