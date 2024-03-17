#ifndef __TOASTERACTIVEOBJECT__
#define __TOASTERACTIVEOBJECT__

#include <iostream>
#include <map>
#include <thread>
#include <memory>
#include <vector>
#include <type_traits>

#include <boost/signals2.hpp>

#include "Actuators.hpp"
#include "Sensors.hpp"
#include "BoostDeadlineTimer.hpp"
#include "Event.hpp"
#include "StateManager.hpp"
#include "ThreadSafeQueue.hpp"

namespace Actor
{

/**
 * @section States Enumeration
 */
enum class StateValue
{
    UNKNOWN = 0,
    ROOT,
    STATE_HEATING,
    STATE_TOASTING,
    STATE_BAKING,
    STATE_DOOR_OPEN,
};
static const std::vector<std::string> stringfier_StateValue{
    "UNKNOWN", "ROOT", "STATE_HEATING", "STATE_TOASTING", "STATE_BAKING", "STATE_DOOR_OPEN",
};
const std::string& stringify(StateValue state);
std::ostream&      operator<<(std::ostream& os, const StateValue& state);

/**
 * @section Forward declaration of the class Toaster
 */
class Toaster;

/**
 * @section States definition
 */
class ToasterSuperState
{
   public:
    ToasterSuperState(Toaster* actor, StateValue state = StateValue::ROOT)
        : m_actor{actor}, m_state_enum{state}
    {
    }
    virtual ~ToasterSuperState()
    {
    }
    virtual void on_entry()
    {
    }
    virtual void on_exit()
    {
    }

    virtual void unhandled_event(IEvent_ptr event)
    {
        (void) event;
        std::cout << "Method called: " << __PRETTY_FUNCTION__ << std::endl;
    }

    virtual void process_event(IEvent_ptr event)
    {
        (void) event;
        std::cout << "Method called: " << __PRETTY_FUNCTION__ << std::endl;
    }

    virtual StateValue type() const
    {
        return m_state_enum;
    }

   protected:
    Toaster* m_actor;

   private:
    StateValue m_state_enum;
};

class StateHeating : public ToasterSuperState
{
   public:
    virtual ~StateHeating() = default;
    StateHeating(Toaster* actor, StateValue state = StateValue::STATE_HEATING)
        : ToasterSuperState(actor, state)
    {
    }
    virtual void on_entry() override;
    virtual void on_exit() override;
    virtual void process_event(IEvent_ptr event) override;
};

class StateToasting : public StateHeating
{
   public:
    virtual ~StateToasting() = default;
    StateToasting(Toaster* actor, StateValue state = StateValue::STATE_TOASTING)
        : StateHeating(actor, state)
    {
    }
    virtual void on_entry() override;
    virtual void on_exit() override;
    virtual void process_event(IEvent_ptr event) override;
};

class StateBaking : public StateHeating
{
   public:
    virtual ~StateBaking() = default;
    StateBaking(Toaster* actor, StateValue state = StateValue::STATE_BAKING)
        : StateHeating(actor, state)
    {
    }
    virtual void on_entry() override;
    virtual void on_exit() override;
    virtual void process_event(IEvent_ptr event) override;
};

class StateDoorOpen : public ToasterSuperState
{
   public:
    virtual ~StateDoorOpen() = default;
    StateDoorOpen(Toaster* actor, StateValue state = StateValue::STATE_DOOR_OPEN)
        : ToasterSuperState(actor, state)
    {
    }
    virtual void on_entry() override;
    virtual void on_exit() override;
    virtual void process_event(IEvent_ptr event) override;
};

/**
 * @section Definition of the class Toaster
 */
class Toaster
{
   public:
    using ToasterSuperState_ptr = std::shared_ptr<ToasterSuperState>;
    Toaster() : m_queue{std::make_shared<SimplestThreadSafeQueue<IEvent_ptr>>()}
    {
    }
    ~Toaster()
    {
    }

    SignalIEvent m_signal;

    std::shared_ptr<StateManager<ToasterSuperState_ptr>>        m_state_manager;
    std::map<StateValue, tree<ToasterSuperState_ptr>::iterator> m_states;
    tree<ToasterSuperState_ptr>::iterator                       m_next_state;

   private:
    void run();

    bool        m_running{false};
    std::thread m_thread;

    std::shared_ptr<SimplestThreadSafeQueue<IEvent_ptr>> m_queue;
};

}  // namespace Actor

#endif