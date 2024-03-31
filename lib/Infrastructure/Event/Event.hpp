#ifndef __EVENT_H_
#define __EVENT_H_

#include <vector>
#include <memory>
#include <cstdint>
#include <typeinfo>

#include <boost/signals2.hpp>

class IEvent
{
   public:
    virtual ~IEvent() = default;
    virtual std::size_t getTypeHash() const
    {
        return typeid(*this).hash_code();
    }

   protected:
    IEvent()
    {
    }
};

using IEvent_ptr      = std::shared_ptr<IEvent>;
using SignatureIEvent = std::function<void(IEvent_ptr)>;
using SignalIEvent    = boost::signals2::signal<void(IEvent_ptr)>;

/***************************************************************************************************
 * CommonEvents
 ***************************************************************************************************/
class EventTimeout : public IEvent
{
   public:
    EventTimeout()
    {
    }
    int timeout;
};

class AlarmTimeout : public IEvent
{
   public:
    AlarmTimeout()
    {
    }
};

/***************************************************************************************************
 * Temp Sensor Events
 ***************************************************************************************************/
class TargetTempReached : public IEvent
{
   public:
    TargetTempReached()
    {
    }
};

class TempAboveTarget : public IEvent
{
   public:
    TempAboveTarget()
    {
    }
};

class TempBelowTarget : public IEvent
{
   public:
    TempBelowTarget()
    {
    }
};

#endif