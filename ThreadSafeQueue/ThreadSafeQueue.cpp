#include <iostream>
#include <memory>
#include <thread>

#include "ThreadSafeQueue.hpp"

std::shared_ptr<spdlog::logger> myLogger = spdlog::stdout_color_mt("console");

void test_queue()
{
    spdlog::set_pattern("[%H:%M:%S %z] [%^%l%$] [thread %t] %v");
    spdlog::set_level(spdlog::level::debug);

    std::shared_ptr<IThreadSafeQueue<int>> queue = std::make_shared<SimplestThreadSafeQueue<int>>();
    auto t1    = std::thread(&IThreadSafeQueue<int>::wait_and_pop, queue.get());
    auto t2    = std::thread(&IThreadSafeQueue<int>::wait_and_pop, queue.get());
    auto t3    = std::thread(&IThreadSafeQueue<int>::wait_and_pop, queue.get());
    auto data1 = int{81};
    auto data2 = int{82};
    auto data3 = int{83};
    std::this_thread::sleep_for(std::chrono::seconds(1));
    myLogger->debug("[test_queue()] Producing data in main thread: data ", data1);
    myLogger->debug("[test_queue()] Producing data in main thread: data ", data2);
    myLogger->debug("[test_queue()] Producing data in main thread: data ", data3);
    queue->put(std::move(data1));
    queue->put(std::move(data2));
    queue->put_prioritized(std::move(data3));
    t1.join();  // Blocks the main thread until t1 thread is finished
    t2.join();  // Blocks the main thread until t2 thread is finished
    t3.join();  // Blocks the main thread until t3 thread is finished
}