#include "Toaster.hpp"

namespace Actor
{
const std::string &stringify(StateValue event)
{
    return stringfier_StateValue[static_cast<int>(event)];
}

std::ostream &operator<<(std::ostream &os, const StateValue &state)
{
    os << stringify(state);
    return os;
}

void StateHeating::on_entry()
{
    std::cout << "Method called: " << __PRETTY_FUNCTION__ << std::endl;
}
void StateHeating::on_exit()
{
    std::cout << "Method called: " << __PRETTY_FUNCTION__ << std::endl;
}
void StateHeating::process_event(IEvent_ptr event)
{
    (void) event;
    std::cout << "Method called: " << __PRETTY_FUNCTION__ << std::endl;
}

void StateToasting::on_entry()
{
    std::cout << "Method called: " << __PRETTY_FUNCTION__ << std::endl;
}
void StateToasting::on_exit()
{
    std::cout << "Method called: " << __PRETTY_FUNCTION__ << std::endl;
}
void StateToasting::process_event(IEvent_ptr event)
{
    (void) event;
    std::cout << "Method called: " << __PRETTY_FUNCTION__ << std::endl;
}

void StateBaking::on_entry()
{
    std::cout << "Method called: " << __PRETTY_FUNCTION__ << std::endl;
}
void StateBaking::on_exit()
{
    std::cout << "Method called: " << __PRETTY_FUNCTION__ << std::endl;
}
void StateBaking::process_event(IEvent_ptr event)
{
    (void) event;
    std::cout << "Method called: " << __PRETTY_FUNCTION__ << std::endl;
}

void StateDoorOpen::on_entry()
{
    std::cout << "Method called: " << __PRETTY_FUNCTION__ << std::endl;
}
void StateDoorOpen::on_exit()
{
    std::cout << "Method called: " << __PRETTY_FUNCTION__ << std::endl;
}
void StateDoorOpen::process_event(IEvent_ptr event)
{
    (void) event;
    std::cout << "Method called: " << __PRETTY_FUNCTION__ << std::endl;
}

void Toaster::start()
{
    m_state_manager->init();
    m_running = true;
    m_thread  = std::thread(&Toaster::run, this);
}
void Toaster::stop()
{
    if (!m_running)
        return;

    // m_queue->put_prioritized(ExternalEventWrapper(InternalEvent::stop_request));

    if (m_thread.joinable())
        m_thread.join();

    m_queue->clear();
}

void Toaster::heater_on()
{
    // std::cout << "Toaster::heater_on()" << std::endl;
    m_heater->turn_on();
}

void Toaster::heater_off()
{
    // std::cout << "Toaster::heater_off()" << std::endl;
    m_heater->turn_off();
}

void Toaster::internal_lamp_on()
{
    // std::cout << "Toaster::internal_lamp_on()" << std::endl;
}

void Toaster::internal_lamp_off()
{
    // std::cout << "Toaster::internal_lamp_off()" << std::endl;
}

void Toaster::arm_time_event(long time)
{
    if (m_timer.status() == DeadlineTimer::Status::running)
    {
        return;
    }
    // std::cout << "Toaster::arm_time_event(long time)" << std::endl;
    m_timer.start(time);
}

void Toaster::arm_time_event(ToastLevel level)
{
    // std::cout << "Toaster::arm_time_event(ToastLevel level)" << std::endl;
    long period = static_cast<long>(level) * 2000;  // Arbitrary hardcoded value
    m_timer.start(period);
}

void Toaster::disarm_time_event()
{
    // std::cout << "Toaster::disarm_time_event()" << std::endl;
    m_timer.stop();
}

void Toaster::set_target_temperature(float temp)
{
    // std::cout << "Toaster::set_target_temperature(float temp) - " << temp << std::endl;
    m_temp_sensor->set_target_temperature(temp);
}

void Toaster::run()
{
    do
    {
        IEvent_ptr current_event = m_queue->wait_and_pop();
        m_state_manager->processEvent(current_event);

        if (m_next_state != m_states[StateValue::UNKNOWN])
        {
            m_state_manager->transitionTo(m_next_state);
            m_next_state = m_states[StateValue::UNKNOWN];
        }
    } while (m_running);
}

void Toaster::timer_callback()
{
    std::shared_ptr<AlarmTimeout> event = std::make_shared<AlarmTimeout>();
    m_queue->put(event);
}

}  // namespace Actor