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
#include "BoostDeadlineTimer.hpp"

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

#define AMBIENT_TEMP 25.0
#define MAX_TEMP 80.0
#define MOCKED_OBJECTS_TIMER_PERIOD 1000

class Heater
{
   public:
    enum class Status
    {
        On,
        Off
    };

   public:
    const float &temperature;

   public:
    Heater()
        : m_status(Status::Off),
          m_temp(AMBIENT_TEMP),
          m_heater_timer{MOCKED_OBJECTS_TIMER_PERIOD, boost::bind(&Heater::callback, this), true},
          temperature(m_temp)
    {
        m_heater_timer.start();
    }

    void turn_on()
    {
        std::cout << "Heater::turn_on()" << std::endl;
        m_status = Status::On;
        // hal_method_to_turn_relay_on(); // Abstracted in this example
    }

    void turn_off()
    {
        std::cout << "Heater::turn_off()" << std::endl;
        m_status = Status::Off;
        // hal_method_to_turn_relay_off(); // Abstracted in this example
    }

   private:
    /* In real world, it wouldn't make sense to manually increment/decrement the temperature. It's
     * just here for the sake of the example making sense */
    void callback()
    {
        if (m_status == Status::On)
        {
            m_temp++;
        }
        else if (m_status == Status::Off && m_temp > AMBIENT_TEMP)
        {
            m_temp--;
        }
    }

   private:
    float         m_temp;
    Status        m_status;
    DeadlineTimer m_heater_timer;
};

/* TODO: Issue#6 */
template <class T>
class TempSensor
{
    using signal_t = boost::signals2::signal<void(const TempSensorEvent &evt)>;

   public:
    struct target_temp_t
    {
        float temp;
        bool  is_set;
    };

   public:
    TempSensor()
        : m_sensor_timer{MOCKED_OBJECTS_TIMER_PERIOD, boost::bind(&TempSensor::callback, this),
                         true}
    {
        m_sensor_timer.start();
    }
    float temp()
    {
        return m_curr_temp;
    }
    void set_target_temp(float temp_to_set_as_target)
    {
        m_target.temp   = temp_to_set_as_target;
        m_target.is_set = true;
    }
    void unset_target_temp()
    {
        m_target.is_set = false;
    }
    bool reached()
    {
        return (m_target.is_set && temp_within_target_range(2.0));
    }
    template <typename F>
    void register_callback(F &&handler)
    {
        m_signal.connect(handler);
    }

   private:
    void callback()
    {
        std::cout << "TempSensor::callback - " << m_curr_temp << std::endl;

        // hal_method_to_read_sensor_temperature(); // Abstracted in this example
        m_curr_temp = m_actuator.temperature;

        if (reached())
        {
            publish_event();
        }

        /* Supposedly, the actuator (heater) is not conscious about it's own temperature. The sensor
        is responsible for checking whether the actuator (heater) should be turned on or off based
        on the current temperature and the limits */
        if (m_curr_temp > MAX_TEMP || reached() || (m_target.is_set && m_curr_temp > m_target.temp))
        {
            m_actuator.turn_off();
        }
        else
        {
            m_actuator.turn_on();
        }
    }

    // -> Method publish (emit) the signal
    void publish_event()
    {
        m_signal(TempSensorEvent{TempSensorEvtType::target_temp_reached});
    }

    bool temp_within_target_range(float range)
    {
        return ((m_curr_temp > m_target.temp - range) && (m_curr_temp < m_target.temp + range));
    }

   private:
    signal_t      m_signal;
    T             m_actuator;
    float         m_curr_temp;
    target_temp_t m_target;
    DeadlineTimer m_sensor_timer;
};

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

   public:
    bool                                      m_running{false};
    std::shared_ptr<tao::GenericToasterState> m_state;
    tao::StateValue                           m_next_state{tao::StateValue::UNKNOWN};
    DoorStatus                                m_door_status;
    std::shared_ptr<IThreadSafeQueue<tao::IncomingEventWrapper>> m_queue;

   public:
    Toaster()
        : m_queue{std::make_shared<SimplestThreadSafeQueue<tao::IncomingEventWrapper>>()},
          m_heater{std::make_shared<Heater>()},
          m_temp_sensor{std::make_shared<TempSensor<Heater>>()},
          m_timer{1000,
                  [this]() {
                      m_queue->put_prioritized(
                          tao::IncomingEventWrapper(tao::InternalEvent::evt_alarm_timeout));
                  },
                  false}
    {
        // load_configs(config);
        set_initial_state(tao::StateValue::STATE_HEATING);
        m_temp_sensor->register_callback(
            boost::bind(&Toaster::put_temp_sensor_event, this, boost::placeholders::_1));
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
    void put_temp_sensor_event(const TempSensorEvent &evt);

    template <class T>
    void generic_event_putter(const T &event)
    {
        m_queue->put(tao::IncomingEventWrapper{event});
    }

    // Methods related to the specifics of this object type (toaster)
    void heater_on();
    void heater_off();
    void internal_lamp_on();
    void internal_lamp_off();
    void arm_time_event(long time);
    void arm_time_event(ToastLevel level);
    void disarm_time_event();
    void set_target_temperature(float temp);
    void unset_target_temperature();

   private:
    std::shared_ptr<Heater>             m_heater;
    std::shared_ptr<TempSensor<Heater>> m_temp_sensor;
    float                               m_target_temp;
    std::thread                         m_thread;
    DeadlineTimer                       m_timer;
};

#endif