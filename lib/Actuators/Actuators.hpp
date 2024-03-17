#ifndef __ACTUATORS__
#define __ACTUATORS__

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

}  // namespace Actuators
#endif