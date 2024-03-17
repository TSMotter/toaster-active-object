#ifndef __THREADSAFEQUEUE__
#define __THREADSAFEQUEUE__

#include <condition_variable>
#include <mutex>
#include <deque>

#include <chrono>
#include <memory>
#include <thread>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

extern std::shared_ptr<spdlog::logger> myLogger;

template <typename T>
class IThreadSafeQueue
{
   public:
    virtual void put(T element)                                             = 0;
    virtual void put_prioritized(T element)                                 = 0;
    virtual T    wait_and_pop()                                             = 0;
    virtual T    wait_and_pop_for(const std::chrono::milliseconds &timeout) = 0;
    virtual bool empty()                                                    = 0;
    virtual void reset()                                                    = 0;
    virtual void clear()                                                    = 0;

   private:
};

template <typename T>
class SimplestThreadSafeQueue : public IThreadSafeQueue<T>
{
   public:
    SimplestThreadSafeQueue()
    {
        myLogger->debug("[SimplestThreadSafeQueue()]");
    }

    virtual void put(T element) override
    {
        {
            std::scoped_lock<std::mutex> lock(m_mutex);
            myLogger->debug("[put(T &&element)] Putting element in back of queue");
            m_queue.push_back(element);
        }
        m_cv.notify_all();
    }
    virtual void put_prioritized(T element) override
    {
        {
            std::scoped_lock<std::mutex> lock(m_mutex);
            myLogger->debug("[put_prioritized(T &&element)] Putting element in front of queue");
            m_queue.push_front(element);
        }
        m_cv.notify_all();
    }
    // Wait without a timeout
    virtual T wait_and_pop() override
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        myLogger->debug("[wait_and_pop()] Waiting for data in background");
        m_cv.wait(lock,
                  [&]()
                  {
                      myLogger->debug("[wait_and_pop()] Checking wait predicate: ",
                                      (!m_queue.empty()));
                      return !m_queue.empty();
                  });
        myLogger->debug("[wait_and_pop()] Finished waiting for data in background");
        T result = m_queue.front();
        m_queue.pop_front();
        lock.unlock();
        return result;
    }
    // Wait with a timeout
    virtual T wait_and_pop_for(const std::chrono::milliseconds &timeout) override
    {
        T                            result;
        std::unique_lock<std::mutex> lock(m_mutex);
        myLogger->debug("[wait_and_pop_for(const std::chrono...] Waiting for data in background");
        /*  The return of wait_for is false if it returns and the predicate is still false */
        if (m_cv.wait_for(
                lock, timeout,
                [&]()
                {
                    myLogger->debug(
                        "[wait_and_pop_for(const std::chrono...] Checking wait predicate: ",
                        (!m_queue.empty()));
                    return !m_queue.empty();
                }))
        {
            myLogger->debug(
                "[wait_and_pop_for(const std::chrono...] Finished waiting for data in background");
            result = m_queue.front();
            m_queue.pop_front();
            lock.unlock();
        }
        return result;
    }
    virtual bool empty() override
    {
        myLogger->debug("[empty()]");
        bool result;
        {
            std::scoped_lock<std::mutex> lock(m_mutex);
            result = m_queue.empty();
        }
        return result;
    }
    virtual void reset() override
    {
        myLogger->debug("[reset()]");
        m_queue = std::deque<T>{};
    }
    virtual void clear() override
    {
        myLogger->debug("[clear()]");
        m_queue.clear();
    }

   private:
    std::deque<T>           m_queue{};
    std::condition_variable m_cv;
    std::mutex              m_mutex;
};

#endif