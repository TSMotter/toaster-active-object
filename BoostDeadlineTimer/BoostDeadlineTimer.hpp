#ifndef __BOOSTDEADLINETIMER__
#define __BOOSTDEADLINETIMER__

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

    void start();
    void stop();

   private:
    void io_context_runner();
    void callback(const boost::system::error_code &err);

   private:
    std::function<void(void)>                m_callback;
    bool                                     m_cyclic;
    bool                                     m_thread_running;
    long                                     m_period;
    std::shared_ptr<boost::asio::io_service> m_service;
    boost::asio::deadline_timer              m_timer;
    std::thread                              m_ioc_thread;
};

#endif