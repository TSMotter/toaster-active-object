#ifndef __TOASTERACTIVEOBJECT__
#define __TOASTERACTIVEOBJECT__

#include <iostream>
#include <map>
#include <thread>
#include <memory>
#include <vector>
#include <type_traits>

#include <boost/asio.hpp>
#include <boost/variant.hpp>
#include <boost/signals2.hpp>

#include "Actuators.hpp"
#include "Sensors.hpp"
#include "Events.hpp"
#include "ThreadSafeQueue.hpp"
#include "BoostDeadlineTimer.hpp"

// Forward declaration
class Toaster;

// namespace toaster active object - tao
namespace tao
{
enum class InternalEvent
{
    unknown,
    evt_stop,
    evt_do_toasting,
    evt_do_baking,
    evt_alarm_timeout,
    evt_target_temp_reached,
    evt_temp_below_target,
    evt_temp_above_target,
    evt_door_open,
    evt_door_close,
    evt_max,
};
static const std::string                unknown_internal_event = "invalid stringify InternalEvent";
static const std::map<int, std::string> stringfier_InternalEvent{
    {static_cast<int>(InternalEvent::unknown), "unknown"},
    {static_cast<int>(InternalEvent::evt_stop), "evt_stop"},
    {static_cast<int>(InternalEvent::evt_do_toasting), "evt_do_toasting"},
    {static_cast<int>(InternalEvent::evt_do_baking), "evt_do_baking"},
    {static_cast<int>(InternalEvent::evt_alarm_timeout), "evt_alarm_timeout"},
    {static_cast<int>(InternalEvent::evt_target_temp_reached), "evt_target_temp_reached"},
    {static_cast<int>(InternalEvent::evt_temp_below_target), "evt_temp_below_target"},
    {static_cast<int>(InternalEvent::evt_temp_above_target), "evt_temp_above_target"},
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
        // Will end up translating from one TempSensorEvtType to one tao::InternalEvent
        temperature_sensor_event,
        // Will end up mapping directly to one of the tao::InternalEvent events
        internal_event,
    };
    struct type_wrapper : public boost::static_visitor<EventType>
    {
        EventType operator()(const ExternalEntityEvent & /*obj*/) const
        {
            return EventType::external_entity_event;
        }
        EventType operator()(const TempSensorEvent & /*obj*/) const
        {
            return EventType::temperature_sensor_event;
        }
        EventType operator()(const InternalEvent & /*obj*/) const
        {
            return EventType::internal_event;
        }
    };
    type_wrapper                                                        wrapper;
    boost::variant<ExternalEntityEvent, TempSensorEvent, InternalEvent> m_event;
    EventType                                                           m_type;

    tao::InternalEvent map_external_entity_event_to_internal_event(
        const ExternalEntityEvent &evt) const;
    tao::InternalEvent map_temperature_sensor_event_to_internal_event(
        const TempSensorEvent &evt) const;

   public:
    IncomingEventWrapper(boost::variant<ExternalEntityEvent, TempSensorEvent, InternalEvent> e)
        : m_event{e}, m_type{e.apply_visitor(wrapper)}
    {
    }

    tao::InternalEvent map_incoming_event_to_internal_event();
};

class GenericToasterState
{
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

   protected:
    Toaster *m_toaster;

   private:
    StateValue m_state;
};

class HeatingSuperState : public GenericToasterState
{
   public:
    virtual ~HeatingSuperState() = default;
    HeatingSuperState(Toaster *tstr, StateValue state = StateValue::STATE_HEATING)
        : GenericToasterState{tstr, state}
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
    ToastingState(Toaster *tstr) : HeatingSuperState{tstr, StateValue::STATE_TOASTING}
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
    BakingState(Toaster *tstr) : HeatingSuperState{tstr, StateValue::STATE_BAKING}
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
    DoorOpenState(Toaster *tstr) : GenericToasterState{tstr, tao::StateValue::STATE_DOOR_OPEN}
    {
    }
    virtual void on_entry() override;
    virtual void process_internal_event(InternalEvent event) override;
    virtual void on_exit() override;
};
}  // namespace tao

namespace DemoObjects
{
#define DEMO_AMBIENT_TEMP 25.0
#define DEMO_MAX_TEMP 80.0
#define DEMO_OBJECTS_TIMER_PERIOD 1000
static float global_curr_temp_inside_toaster = DEMO_AMBIENT_TEMP;

class HeaterDemo : public Actuators::IHeater
{
   public:
    HeaterDemo(float &toaster_temp = global_curr_temp_inside_toaster)
        : m_ref_curr_toaster_temp(toaster_temp),
          m_temp(DEMO_AMBIENT_TEMP),
          m_heater_timer{DEMO_OBJECTS_TIMER_PERIOD, boost::bind(&HeaterDemo::callback, this), true}
    {
        // Initializes common protected members from interface
        m_status = Status::Off;

        m_heater_timer.start();
    }

    void turn_on() override
    {
        m_status = Status::On;
    }

    void turn_off() override
    {
        m_status = Status::Off;
    }

   private:
    void callback()
    {
        if (m_status == Status::On && m_temp < DEMO_MAX_TEMP)
        {
            m_temp++;
        }
        else if (m_status == Status::Off && m_temp > DEMO_AMBIENT_TEMP)
        {
            m_temp--;
        }
        // Toaster current internal temperature follows current heater temperature...
        m_ref_curr_toaster_temp = m_temp;
    }

   private:
    float        &m_ref_curr_toaster_temp;
    float         m_temp;
    DeadlineTimer m_heater_timer;
};
using TempSensorSpecializedCallback =
    Sensors::ITempSensor<std::function<void(const TempSensorEvent &)>>;
class TempSensorDemo : public TempSensorSpecializedCallback
{
    using signal_t = boost::signals2::signal<void(const TempSensorEvent &evt)>;

   public:
    TempSensorDemo(const float &toaster_temp = global_curr_temp_inside_toaster, float error = 2.0f)
        : m_ref_curr_toaster_temp(toaster_temp),
          m_error(error),
          m_sensor_timer{DEMO_OBJECTS_TIMER_PERIOD, boost::bind(&TempSensorDemo::callback, this),
                         true}
    {
        // Initializes common protected members from interface
        m_status      = Status::Off;
        m_curr_temp   = DEMO_AMBIENT_TEMP;
        m_target_temp = DEMO_AMBIENT_TEMP;

        m_sensor_timer.start();
    }

    // Overloading the initialize method
    void initialize(std::function<void(const TempSensorEvent &)> cb) override
    {
        register_callback(cb);
    }

    void turn_on() override
    {
        m_status = Status::On;
    }
    void turn_off() override
    {
        m_status = Status::Off;
    }

    float get_temperature() const override
    {
        return m_curr_temp;
    }

    void set_target_temperature(float temp) override
    {
        m_target_temp = temp;
    }

    Status get_status() const override
    {
        return m_status;
    }

   private:
    template <typename F>
    void register_callback(F &&handler)
    {
        m_signal.connect(handler);
    }

    void callback()
    {
        m_curr_temp = m_ref_curr_toaster_temp;

        if ((m_curr_temp > m_target_temp - m_error) && (m_curr_temp < m_target_temp + m_error))
        {
            publish_event(TempSensorEvent{TempSensorEvtType::target_temp_reached});
        }
        else if (m_curr_temp > m_target_temp - m_error)
        {
            publish_event(TempSensorEvent{TempSensorEvtType::temp_above_target});
        }
        else if (m_curr_temp < m_target_temp + m_error)
        {
            publish_event(TempSensorEvent{TempSensorEvtType::temp_below_target});
        }
    }

    // -> Method publish (emit) the signal
    void publish_event(const TempSensorEvent &evt)
    {
        m_signal(evt);
    }

   private:
    const float  &m_ref_curr_toaster_temp;
    signal_t      m_signal;
    float         m_error;
    DeadlineTimer m_sensor_timer;
};
}  // namespace DemoObjects

/* TODO: Issue#7 - Implement unit tests for Toaster*/
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

    Toaster(std::shared_ptr<Actuators::IHeater>                         htr,
            std::shared_ptr<DemoObjects::TempSensorSpecializedCallback> ssr)
        : m_queue{std::make_shared<SimplestThreadSafeQueue<tao::IncomingEventWrapper>>()},
          m_heater{htr},
          m_temp_sensor{ssr},
          m_timer{1000, boost::bind(&Toaster::timer_callback, this), false}
    {
        set_initial_state(tao::StateValue::STATE_HEATING);
        m_temp_sensor->initialize(
            boost::bind(&Toaster::put_temp_sensor_event, this, boost::placeholders::_1));
    }

    ~Toaster()
    {
        stop();
        m_queue->clear();
    }

    void set_next_state(tao::StateValue new_state);
    void set_state(tao::StateValue new_state);
    void transition_state();
    void state_machine_iteration();
    void state_machine_iteration(tao::InternalEvent evt);
    void set_initial_state(tao::StateValue new_state);

    void run();
    void start();
    void stop();

    void put_external_entity_event(const ExternalEntityEvent &evt);
    void put_temp_sensor_event(const TempSensorEvent &evt);

    template <class T>
    void generic_event_putter(const T &event)
    {
        m_queue->put(tao::IncomingEventWrapper{event});
    }

    void heater_on();
    void heater_off();
    void internal_lamp_on();
    void internal_lamp_off();
    void arm_time_event(long time);
    void arm_time_event(ToastLevel level);
    void disarm_time_event();
    void set_target_temperature(float temp);

    bool                                      m_running{false};
    std::shared_ptr<tao::GenericToasterState> m_state;
    tao::StateValue                           m_next_state{tao::StateValue::UNKNOWN};
    DoorStatus                                m_door_status;
    std::shared_ptr<IThreadSafeQueue<tao::IncomingEventWrapper>> m_queue;

   private:
    void timer_callback()
    {
        m_queue->put_prioritized(tao::IncomingEventWrapper(tao::InternalEvent::evt_alarm_timeout));
    }

   private:
    std::shared_ptr<Actuators::IHeater>                         m_heater;
    std::shared_ptr<DemoObjects::TempSensorSpecializedCallback> m_temp_sensor;
    float                                                       m_target_temp;
    std::thread                                                 m_thread;
    DeadlineTimer                                               m_timer;
};

#endif