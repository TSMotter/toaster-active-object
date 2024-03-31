#include "Sensors.hpp"

namespace Sensors
{
// Overloading the initialize method
void DemoTempSensor::initialize(SignatureIEvent cb)
{
    register_callback(cb);
}

void DemoTempSensor::turn_on()
{
    m_status = Status::On;
}
void DemoTempSensor::turn_off()
{
    m_status = Status::Off;
}

float DemoTempSensor::get_temperature() const
{
    return m_curr_temp;
}

void DemoTempSensor::set_target_temperature(float temp)
{
    m_target_temp = temp;
}

ITempSensor::Status DemoTempSensor::get_status() const
{
    return m_status;
}

void DemoTempSensor::callback()
{
    m_curr_temp = m_ref_curr_toaster_temp;

    if ((m_curr_temp > m_target_temp - m_error) && (m_curr_temp < m_target_temp + m_error))
    {
        std::shared_ptr<TargetTempReached> event = std::make_shared<TargetTempReached>();
        m_signal(event);
    }
    else if (m_curr_temp > m_target_temp - m_error)
    {
        std::shared_ptr<TempAboveTarget> event = std::make_shared<TempAboveTarget>();
        m_signal(event);
    }
    else if (m_curr_temp < m_target_temp + m_error)
    {
        std::shared_ptr<TempBelowTarget> event = std::make_shared<TempBelowTarget>();
        m_signal(event);
    }
}
}