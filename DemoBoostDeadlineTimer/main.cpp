#include <iostream>
#include <thread>

#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

//#define USE_SPDLOG


#if defined(USE_SPDLOG)

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
class MyCoolLogger
{
   public:
    MyCoolLogger()
    {
        m_logger = spdlog::stdout_color_mt("console");
        spdlog::set_pattern("[%H:%M:%S:%e] [%^%l%$] [thread %t] %v");
        spdlog::set_level(spdlog::level::debug);
    }

    void Logdebug(const std::string &str)
    {
        m_logger->debug(str);
    }
    void Loginfo(const std::string &str)
    {
        m_logger->info(str);
    }

   private:
    std::shared_ptr<spdlog::logger> m_logger;
};

#else

class MyCoolLogger
{
   public:
    MyCoolLogger()
    {
    }
    void Logdebug(const std::string &str)
    {
        std::cout << str << std::endl;
    }
    void Loginfo(const std::string &str)
    {
        std::cout << str << std::endl;
    }
};

#endif


class DeadlineTimer : public MyCoolLogger
{
   public:
    DeadlineTimer(long T, std::function<void(void)> cb, bool cyclic = false)
        : m_service{std::make_shared<boost::asio::io_service>()},
          m_timer(*m_service),
          m_period(T),
          m_callback(cb),
          m_cyclic(cyclic),
          m_thread_running(false)
    {
        Logdebug("DeadlineTimer");
    }

    ~DeadlineTimer()
    {
        Logdebug("~DeadlineTimer()");
        stop();
    }

    void start()
    {
        Logdebug("void start()");
        m_timer.expires_from_now(boost::posix_time::milliseconds(m_period));
        m_timer.async_wait(
            boost::bind(&DeadlineTimer::callback, this, boost::asio::placeholders::error));

        if (!m_thread_running)
        {
            m_ioc_thread = std::thread(&DeadlineTimer::io_context_runner, this);
        }
    }

    void stop()
    {
        Logdebug("void stop()");
        m_timer.cancel();
        m_service->stop();
        if (m_ioc_thread.joinable())
        {
            m_ioc_thread.join();
            m_thread_running = false;
        }
    }

   private:
    void io_context_runner()
    {
        Logdebug("io_context_runner()");
        m_thread_running = true;
        if (m_service)
        {
            m_service->run();
        }
    }

    void callback(const boost::system::error_code &err)
    {
        Logdebug("void callback()");
        if (err)
        {
            return;
        }
        m_callback();
        if (m_cyclic)
        {
            start();
        }
    }

   private:
    std::function<void(void)>                m_callback;
    bool                                     m_cyclic;
    bool                                     m_thread_running;
    long                                     m_period;
    std::shared_ptr<boost::asio::io_service> m_service;
    boost::asio::deadline_timer              m_timer;
    std::thread                              m_ioc_thread;
};


int main()
{
    DeadlineTimer myTimer{500, []() { std::cout << "Callbacked" << std::endl; }, true};

    std::this_thread::sleep_for(std::chrono::seconds(2));
    myTimer.start();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    myTimer.stop();


    return 0;
}