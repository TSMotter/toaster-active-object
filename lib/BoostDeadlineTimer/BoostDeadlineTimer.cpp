#include "BoostDeadlineTimer.hpp"

#define USE_SPDLOG

#if defined(USE_SPDLOG)

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

class MyCoolLogger
{
   public:
    MyCoolLogger()
    {
        m_logger = spdlog::stdout_color_mt("BoostDeadlineTimer");
        spdlog::set_pattern("[%H:%M:%S:%e][%n][%^%l%$][thread_id=%t] %v");
        spdlog::set_level(spdlog::level::info);
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

static MyCoolLogger myLocalLogger{};

DeadlineTimer::DeadlineTimer(long T, std::function<void(void)> cb, bool cyclic)
    : m_service{std::make_shared<boost::asio::io_service>()},
      m_worker{std::unique_ptr<boost::asio::io_service::work>(
          new boost::asio::io_service::work{*m_service})},
      m_timer(*m_service),
      m_period(T),
      m_status(Status::stopped),
      m_callback(cb),
      m_cyclic(cyclic)
{
    myLocalLogger.Logdebug("[Constructor()]");
    m_ioc_thread = std::thread(&DeadlineTimer::io_context_runner, this);
}

DeadlineTimer::~DeadlineTimer()
{
    myLocalLogger.Logdebug("[Destructor()]");
    m_worker.reset();
    m_service->stop();
    if (m_ioc_thread.joinable())
    {
        myLocalLogger.Logdebug("[void stop()] - thread.join");
        m_ioc_thread.join();
    }
}

void DeadlineTimer::start()
{
    myLocalLogger.Logdebug("[void start()]");
    m_timer.expires_from_now(boost::posix_time::milliseconds(m_period));
    m_timer.async_wait(
        boost::bind(&DeadlineTimer::callback, this, boost::asio::placeholders::error));
    m_status = Status::running;
}

void DeadlineTimer::start(long T)
{
    myLocalLogger.Logdebug("[void start(long T)]");
    m_period = T;
    m_timer.expires_from_now(boost::posix_time::milliseconds(m_period));
    m_timer.async_wait(
        boost::bind(&DeadlineTimer::callback, this, boost::asio::placeholders::error));
    m_status = Status::running;
}

void DeadlineTimer::start(long T, bool cyclic)
{
    myLocalLogger.Logdebug("[void start(long T)]");
    m_period = T;
    m_cyclic = cyclic;
    m_timer.expires_from_now(boost::posix_time::milliseconds(m_period));
    m_timer.async_wait(
        boost::bind(&DeadlineTimer::callback, this, boost::asio::placeholders::error));
    m_status = Status::running;
}

void DeadlineTimer::stop()
{
    myLocalLogger.Logdebug("[void stop()]");
    m_status = Status::stopped;
    m_timer.cancel();
}

DeadlineTimer::Status DeadlineTimer::status()
{
    myLocalLogger.Logdebug("[DeadlineTimer::Status status()]");
    return m_status;
}

void DeadlineTimer::io_context_runner()
{
    myLocalLogger.Logdebug("[io_context_runner()]");
    if (m_service)
    {
        m_service->run();
    }
}

void DeadlineTimer::callback(const boost::system::error_code &err)
{
    myLocalLogger.Logdebug("[void callback()]");
    if (err)
    {
        return;
    }
    m_callback();
    if (m_cyclic && m_status == Status::running)
    {
        start();
    }
    else
    {
        m_status = Status::stopped;
    }
}
