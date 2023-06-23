#ifndef __SENSORS__
#define __SENSORS__

#include "Events.hpp"

namespace Sensors
{

class ITempSensor
{
   public:
    enum class Status
    {
        On,
        Off
    };

    virtual void   turn_on()                          = 0;
    virtual void   turn_off()                         = 0;
    virtual float  get_temperature() const            = 0;
    virtual void   set_target_temperature(float temp) = 0;
    virtual Status get_status() const
    {
        return m_status;
    }

   protected:
    Status m_status;
    float  m_curr_temp;
    float  m_target_temp;
};

template <class T>
class ISmartTempSensor : public ITempSensor
{
   protected:
    std::shared_ptr<T> m_actuator;
};

}  // namespace Sensors

#endif