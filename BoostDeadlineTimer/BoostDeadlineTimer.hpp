#ifndef __BOOSTDEADLINETIMER__
#define __BOOSTDEADLINETIMER__

#include <thread>
#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

class DeadlineTimer
{
   public:
    enum class Status
    {
        running,
        stopped
    };

    DeadlineTimer(long T, std::function<void(void)> cb, bool cyclic);

    ~DeadlineTimer();

    void   start();
    void   start(long T);
    void   start(long T, bool cyclic);
    void   stop();
    Status status();

   private:
    void io_context_runner();
    void callback(const boost::system::error_code &err);

    std::function<void(void)>                      m_callback;
    bool                                           m_cyclic;
    long                                           m_period;
    Status                                         m_status;
    std::shared_ptr<boost::asio::io_service>       m_service;
    std::unique_ptr<boost::asio::io_service::work> m_worker;
    boost::asio::deadline_timer                    m_timer;
    std::thread                                    m_ioc_thread;
};

#endif