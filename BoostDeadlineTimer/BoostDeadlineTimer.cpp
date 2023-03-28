#include "BoostDeadlineTimer.hpp"

void DeadlineTimer::start()
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

void DeadlineTimer::stop()
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

void DeadlineTimer::io_context_runner()
{
    Logdebug("io_context_runner()");
    m_thread_running = true;
    if (m_service)
    {
        m_service->run();
    }
}

void DeadlineTimer::callback(const boost::system::error_code &err)
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
