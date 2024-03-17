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
        /* clang-format off */
        tree<ToasterSuperState_ptr> tree;
        tree.set_head(std::make_shared<ToasterSuperState>(this));
        m_states[StateValue::ROOT] = tree.begin();
        m_states[StateValue::STATE_HEATING] = tree.append_child(m_states[StateValue::ROOT], std::make_shared<StateHeating>(this, StateValue::STATE_HEATING));
        m_states[StateValue::STATE_TOASTING] = tree.append_child(m_states[StateValue::STATE_HEATING], std::make_shared<StateToasting>(this, StateValue::STATE_TOASTING));
        m_states[StateValue::STATE_BAKING] = tree.append_child(m_states[StateValue::STATE_HEATING], std::make_shared<StateBaking>(this, StateValue::STATE_BAKING));
        m_states[StateValue::STATE_DOOR_OPEN] = tree.append_child(m_states[StateValue::ROOT], std::make_shared<StateDoorOpen>(this, StateValue::STATE_DOOR_OPEN));
        m_states[StateValue::UNKNOWN] = tree.end();

        m_next_state = m_states[StateValue::UNKNOWN];

        m_state_manager = std::make_shared<StateManager<ToasterSuperState_ptr>>(std::move(tree), m_states[StateValue::STATE_HEATING]);
        /* clang-format on */
    }
    ~Toaster()
    {
    }

    void start();
    void stop();

    void callback_IEvent(IEvent_ptr event)
    {
        m_queue->put(event);
    }

    template <class T>
    boost::signals2::connection connect_callback_for_signal_emited_from_A(T&& handler)
    {
        return m_signal.connect(handler);
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