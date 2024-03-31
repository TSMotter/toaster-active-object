#ifndef __ACTUATORS__
#define __ACTUATORS__

#include "BoostDeadlineTimer.hpp"
#include "DemoObjects.hpp"

namespace Actuators
{

class IHeater
{
   public:
    enum class Status
    {
        On,
        Off
    };

    virtual void   turn_on()  = 0;
    virtual void   turn_off() = 0;
    virtual Status get_status() const
    {
        return m_status;
    }

   protected:
    Status m_status;
};

class DemoHeater : public IHeater
{
   public:
    DemoHeater(float &toaster_temp = nGLOBAL_CURR_TEMP_INSIDE_TOASTER)
        : m_ref_curr_toaster_temp(toaster_temp),
          m_temp(nDEMO_AMBIENT_TEMP),
          m_heater_timer{nDEMO_OBJECTS_TIMER_PERIOD, boost::bind(&DemoHeater::callback, this), true}
    {
        // Initializes common protected members from interface
        m_status = IHeater::Status::Off;

        m_heater_timer.start();
    }

    void turn_on() override;
    void turn_off() override;

   private:
    void callback();

   private:
    float        &m_ref_curr_toaster_temp;
    float         m_temp;
    DeadlineTimer m_heater_timer;
};

}  // namespace Actuators
#endif