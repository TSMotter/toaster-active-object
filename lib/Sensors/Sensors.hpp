#ifndef __SENSORS__
#define __SENSORS__

#include "Event.hpp"
#include "BoostDeadlineTimer.hpp"
#include "DemoObjects.hpp"

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

    virtual void   initialize(SignatureIEvent cb)     = 0;
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

class DemoTempSensor : public ITempSensor
{
   public:
    DemoTempSensor(const float &toaster_temp = nGLOBAL_CURR_TEMP_INSIDE_TOASTER, float error = 2.0f)
        : m_ref_curr_toaster_temp(toaster_temp),
          m_error(error),
          m_sensor_timer{nDEMO_OBJECTS_TIMER_PERIOD, boost::bind(&DemoTempSensor::callback, this),
                         true}
    {
        // Initializes common protected members from interface
        m_status      = Status::Off;
        m_curr_temp   = nDEMO_AMBIENT_TEMP;
        m_target_temp = nDEMO_AMBIENT_TEMP;

        m_sensor_timer.start();
    }

    // Overloading the initialize method
    void initialize(SignatureIEvent cb) override;
    void turn_on() override;
    void turn_off() override;
    float get_temperature() const override;
    void set_target_temperature(float temp) override;
    ITempSensor::Status get_status() const override;

   private:
    template <typename F>
    void register_callback(F &&handler)
    {
        m_signal.connect(handler);
    }

    void callback();

   private:
    const float  &m_ref_curr_toaster_temp;
    SignalIEvent  m_signal;
    float         m_error;
    DeadlineTimer m_sensor_timer;
};

}  // namespace Sensors

#endif