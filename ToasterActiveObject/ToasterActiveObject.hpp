#ifndef __TOASTERACTIVEOBJECT__
#define __TOASTERACTIVEOBJECT__

#include <iostream>
#include <map>
#include <thread>
#include <memory>
#include <vector>

#include <boost/asio.hpp>
#include <boost/variant.hpp>
#include <boost/signals2.hpp>

#include "Events.hpp"
#include "ThreadSafeQueue.hpp"

class Toaster;

// namespace toaster active object - tao
namespace tao
{
enum class InternalEvent
{
    unknown,
    evt_stop,
    evt_do_toasting,
    evt_alarm_timeout,
    evt_do_baking,
    evt_door_open,
    evt_door_close,
    evt_max,
};
static const std::string                unknown_internal_event = "invalid stringify InternalEvent";
static const std::map<int, std::string> stringfier_InternalEvent{
    {static_cast<int>(InternalEvent::unknown), "unknown"},
    {static_cast<int>(InternalEvent::evt_stop), "evt_stop"},
    {static_cast<int>(InternalEvent::evt_do_toasting), "evt_do_toasting"},
    {static_cast<int>(InternalEvent::evt_alarm_timeout), "evt_alarm_timeout"},
    {static_cast<int>(InternalEvent::evt_do_baking), "evt_do_baking"},
    {static_cast<int>(InternalEvent::evt_door_open), "evt_door_open"},
    {static_cast<int>(InternalEvent::evt_door_close), "evt_door_close"},
};
const std::string &stringify(InternalEvent event);
std::ostream      &operator<<(std::ostream &os, const tao::InternalEvent &event);


enum class StateValue
{
    UNKNOWN = 0,
    STATE_HEATING,
    STATE_TOASTING,
    STATE_BAKING,
    STATE_DOOR_OPEN,
};
static const std::vector<std::string> stringfier_StateValue{
    "UNKNOWN", "STATE_HEATING", "STATE_TOASTING", "STATE_BAKING", "STATE_DOOR_OPEN",
};
const std::string &stringify(StateValue state);
std::ostream      &operator<<(std::ostream &os, const tao::StateValue &state);

class IncomingEventWrapper
{
   private:
    enum class EventType
    {
        unknown,
        // Will end up translating from one ExternalEntityEvtType to one tao::InternalEvent
        external_entity_event,
        // Will end up mapping directly to one of the tao::InternalEvent events
        internal_event,
    };
    struct type_wrapper : public boost::static_visitor<EventType>
    {
        EventType operator()(const InternalEvent & /*obj*/) const
        {
            return EventType::internal_event;
        }
        EventType operator()(const ExternalEntityEvent & /*obj*/) const
        {
            return EventType::external_entity_event;
        }
    };
    type_wrapper                                       wrapper;
    boost::variant<InternalEvent, ExternalEntityEvent> m_event;
    EventType                                          m_type;

    tao::InternalEvent map_external_entity_event_to_internal_event(
        const ExternalEntityEvent &external_entity_event) const;

   public:
    IncomingEventWrapper(boost::variant<InternalEvent, ExternalEntityEvent> e)
        : m_event{e}, m_type{e.apply_visitor(wrapper)}
    {
    }

    tao::InternalEvent map_external_to_internal_event();
};

class GenericToasterState
{
   protected:
    Toaster *m_toaster;

   private:
    StateValue m_state;

   public:
    GenericToasterState(Toaster *tstr, StateValue state = StateValue::UNKNOWN)
        : m_toaster{tstr}, m_state{state}
    {
    }

    virtual ~GenericToasterState() = default;

    virtual void on_entry()
    {
    }
    virtual void on_exit()
    {
    }
    virtual void       set_next_state(StateValue state);
    virtual void       unhandled_event(InternalEvent event);
    virtual void       process_internal_event(InternalEvent event) = 0;
    virtual StateValue type() const
    {
        return m_state;
    }
};

class HeatingSuperState : public GenericToasterState
{
   public:
    virtual ~HeatingSuperState() = default;
    HeatingSuperState(Toaster *tstr) : GenericToasterState(tstr, StateValue::STATE_HEATING)
    {
    }
    virtual void on_entry() override;
    virtual void unhandled_event(InternalEvent event) override;
    virtual void process_internal_event(InternalEvent event) override;
    virtual void on_exit() override;
};

class ToastingState : public HeatingSuperState
{
   public:
    virtual ~ToastingState()
    {
    }
    ToastingState(Toaster *tstr) : HeatingSuperState(tstr)
    {
    }
    virtual void on_entry() override;
    virtual void process_internal_event(InternalEvent event) override;
    virtual void on_exit() override;
};
class BakingState : public HeatingSuperState
{
   public:
    virtual ~BakingState()
    {
    }
    BakingState(Toaster *tstr) : HeatingSuperState(tstr)
    {
    }
    virtual void on_entry() override;
    virtual void process_internal_event(InternalEvent event) override;
    virtual void on_exit() override;
};
class DoorOpenState : public GenericToasterState
{
   public:
    virtual ~DoorOpenState() = default;
    DoorOpenState(Toaster *tstr) : GenericToasterState(tstr, tao::StateValue::STATE_DOOR_OPEN)
    {
    }
    virtual void on_entry() override;
    virtual void process_internal_event(InternalEvent event) override;
    virtual void on_exit() override;
};
}  // namespace tao

class Toaster
{
   public:
    enum class DoorStatus
    {
        opened,
        closed
    };

    enum class ToastLevel
    {
        bread,
        hot_bread,
        normal_toast,
        slightly_overcooked_toast,
        overcooked_toast,
        charcoal,
    };

    bool                                      m_running{false};
    std::shared_ptr<tao::GenericToasterState> m_state;
    tao::StateValue                           m_next_state{tao::StateValue::UNKNOWN};
    DoorStatus                                m_door_status;
    std::shared_ptr<IThreadSafeQueue<tao::IncomingEventWrapper>> m_queue;

   public:
    Toaster() : m_queue{std::make_shared<SimplestThreadSafeQueue<tao::IncomingEventWrapper>>()}
    {
        // load_configs(config);
        set_initial_state(tao::StateValue::STATE_HEATING);
    }
    ~Toaster()
    {
        stop();
        m_queue->clear();
    }

    // Methods related to the general operation of any object of this architecture
    void set_next_state(tao::StateValue new_state);
    void set_state(tao::StateValue new_state);
    void transition_state();
    void state_machine_iteration();
    void state_machine_iteration(tao::InternalEvent evt);
    void set_initial_state(tao::StateValue new_state);
    // Thread related methods
    void run();
    void start();
    void stop();

    void put_external_entity_event(const ExternalEntityEvent &evt);
    template <class T>
    void generic_event_putter(const T &event)
    {
        m_queue->put(tao::IncomingEventWrapper(event));
    }

    // Methods related to the specifics of this object type (toaster)
    void  heater_on();
    void  heater_off();
    void  internal_lamp_on();
    void  internal_lamp_off();
    void  arm_time_event(uint32_t time);
    void  arm_time_event(ToastLevel level);
    void  disarm_time_event();
    void  set_target_temperature(float temp);
    float check_temp();
    bool  temp_reached();

   private:
    float       m_temp;
    float       m_target_temp;
    std::thread m_thread;
};

#endif