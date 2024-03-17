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
}  // namespace Actor