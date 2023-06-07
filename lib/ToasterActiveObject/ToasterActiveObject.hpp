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
    Heater()
        : m_status(Status::Off),
          m_temp(AMBIENT_TEMP),
          m_heater_timer{MOCKED_OBJECTS_TIMER_PERIOD, boost::bind(&Heater::callback, this), true}
    {
        m_heater_timer.start();
    }

    void turn_on()
    {
        std::cout << "Heater::turn_on()" << std::endl;
        m_status = Status::On;
        // hal_method_to_turn_relay_on(); // Something like this would be done in the real world
    }

    void turn_off()
    {
        std::cout << "Heater::turn_off()" << std::endl;
        m_status = Status::Off;
        // hal_method_to_turn_relay_off(); // Something like this would be done in the real world
    }

    float temperature()
    {
        // hal_method_to_read_temperature(); // Something like this would be done in the real world
        return m_temp;
    }

   private:
    /* In the real world, it wouldn't make sense to manually increment/decrement the temperature.
     * It's just here for the sake of the example making sense */
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
    /* In the real world, the actuator itself wouldn't be concious of it's own temperature. It's
     * just here for the sake of the example making sense */
    float         m_temp;
    Status        m_status;
    DeadlineTimer m_heater_timer;
};

/* SFINAE (Substitution Failure Is Not An Error) is a technique in C++ that can be used to enforce
that the class used for specialization of a template in fact has the required methods and members.

This SFINAE method using std::enable_if, which leads to a more readable approach.
Also, this method allows to check not only that the method "temperature()" exists on the
specialization class, but also checks that it's return type is the one expected, in this case a
float */
template <typename T>
struct ActuatorIsValidForTempSensor
{
    /* - The first test function uses std::enable_if in the return type to conditionally enable it
    based on the type of std::declval<U>().temperature().
    - We use std::is_same to check if the type of std::declval<U>().temperature() is float.
    - If it is, we define the return type as std::true_type, indicating that U has a valid
    temperature() member function. */
    template <typename U>
    static typename std::enable_if<
        std::is_same<decltype(std::declval<U>().temperature()), float>::value, std::true_type>::type
    test(int);

    /* This second test function is a fallback option that will be used if the first test function
     * cannot be matched during overload resolution. */
    template <typename U>
    static std::false_type test(...);

    static constexpr bool value = std::is_same<decltype(test<T>(0)), std::true_type>::value;
};


template <class T>
class TempSensor
{
    static_assert(
        ActuatorIsValidForTempSensor<T>::value,
        "The class used for specialization of TempSensor does not match the requirements");

    using signal_t = boost::signals2::signal<void(const TempSensorEvent &evt)>;

   public:
    /* This public constant reference to a float makes it so that the temperature can be accessed
    from the outside as if it was a read-only attribute */
    const float &temperature;

   public:
    TempSensor(std::shared_ptr<T> act, float error = 2.0f)
        : m_actuator(act),
          m_error(error),
          m_sensor_timer{MOCKED_OBJECTS_TIMER_PERIOD, boost::bind(&TempSensor::callback, this),
                         true},
          m_target_temp(AMBIENT_TEMP),
          temperature(m_curr_temp)
    {
        m_sensor_timer.start();
    }
    void set_target_temp(float temp_to_set_as_target)
    {
        m_target_temp = temp_to_set_as_target;
    }
    template <typename F>
    void register_callback(F &&handler)
    {
        m_signal.connect(handler);
    }

   private:
    void callback()
    {
        std::cout << "TempSensor::callback - " << m_curr_temp << "/" << m_target_temp << std::endl;

        // hal_method_to_read_sensor_temperature(); // Abstracted in this example
        m_curr_temp = m_actuator->temperature();

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
    signal_t           m_signal;
    std::shared_ptr<T> m_actuator;
    float              m_curr_temp;
    float              m_error;
    float              m_target_temp;
    DeadlineTimer      m_sensor_timer;
};

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
          m_temp_sensor{std::make_shared<TempSensor<Heater>>(m_heater, 2.0f)},
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

   private:
    std::shared_ptr<Heater>             m_heater;
    std::shared_ptr<TempSensor<Heater>> m_temp_sensor;
    float                               m_target_temp;
    std::thread                         m_thread;
    DeadlineTimer                       m_timer;
};

#endif