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

    virtual void initialize(...)
    {
        std::cout << "virtual void initialize(...)" << std::endl;
    }
    virtual void   turn_on()                          = 0;
    virtual void   turn_off()                         = 0;
    virtual float  get_temperature() const            = 0;
    virtual void   set_target_temperature(float temp) = 0;
    virtual Status get_status() const                 = 0;

   protected:
    Status m_status;
    float  m_curr_temp;
    float  m_target_temp;
};

}  // namespace Sensors

#endif