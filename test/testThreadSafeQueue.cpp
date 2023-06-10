#include <gtest/gtest.h>
#include <thread>
#include <vector>

#include "ThreadSafeQueue.hpp"

// Fixture definition
class ThreadSafeQueueFixture : public ::testing::Test
{
   protected:
    ThreadSafeQueueFixture() : m_queue{std::make_shared<SimplestThreadSafeQueue<int>>()}
    {
        // You can do set-up work for each test here.
    }

    ~ThreadSafeQueueFixture()
    {
        // You can do clean-up work that doesn't throw exceptions here.
        m_queue->clear();
    }

    std::shared_ptr<IThreadSafeQueue<int>> m_queue;
};

TEST_F(ThreadSafeQueueFixture, TestPut)
{
    m_queue->put(std::move(int{10}));
    m_queue->put(std::move(int{20}));
    m_queue->put(std::move(int{30}));

    auto runner = [](std::shared_ptr<IThreadSafeQueue<int>> q, int reference_element)
    {
        std::shared_ptr<int> popped_element;
        popped_element = q->wait_and_pop();
        ASSERT_EQ(reference_element, *popped_element);
    };

    std::vector<std::thread> threads;
    threads.emplace_back(runner, m_queue, 10);
    threads.emplace_back(runner, m_queue, 20);
    threads.emplace_back(runner, m_queue, 30);
    for (auto &thread : threads)
        thread.join();
}

TEST_F(ThreadSafeQueueFixture, TestPutPrioritized)
{
    m_queue->put(std::move(int{10}));
    m_queue->put(std::move(int{20}));
    m_queue->put_prioritized(std::move(int{30}));

    auto runner = [](std::shared_ptr<IThreadSafeQueue<int>> q, int reference_element)
    {
        std::shared_ptr<int> popped_element;
        popped_element = q->wait_and_pop();
        ASSERT_EQ(reference_element, *popped_element);
    };

    std::vector<std::thread> threads;
    threads.emplace_back(runner, m_queue, 30);
    threads.emplace_back(runner, m_queue, 10);
    threads.emplace_back(runner, m_queue, 20);
    for (auto &thread : threads)
        thread.join();
}