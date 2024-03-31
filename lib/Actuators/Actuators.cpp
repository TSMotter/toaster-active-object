#include "Actuators.hpp"

namespace Actuators
{
void DemoHeater::turn_on()
{
    m_status = IHeater::Status::On;
}

void DemoHeater::turn_off()
{
    m_status = IHeater::Status::Off;
}

void DemoHeater::callback()
{
    if (m_status == IHeater::Status::On && m_temp < nDEMO_MAX_TEMP)
    {
        m_temp++;
    }
    else if (m_status == IHeater::Status::Off && m_temp > nDEMO_AMBIENT_TEMP)
    {
        m_temp--;
    }
    // Toaster current internal temperature follows current heater temperature...
    m_ref_curr_toaster_temp = m_temp;
}
}  // namespace Actuators