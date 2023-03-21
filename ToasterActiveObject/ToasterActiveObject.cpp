#include <iostream>

#include "ToasterActiveObject.hpp"

/* *************************************************************************************************
Implementations of different stringfy functions and "<<" operators
************************************************************************************************* */

const std::string &tao::stringify(tao::InternalEvent event)
{
    auto it = stringfier_InternalEvent.find(static_cast<int>(event));
    if (it != stringfier_InternalEvent.end())
    {
        return it->second;
    }
    return unknown_internal_event;
}

std::ostream &operator<<(std::ostream &os, const tao::InternalEvent &event)
{
    os << stringify(event);
    return os;
}

const std::string &tao::stringify(tao::StateValue event)
{
    return stringfier_StateValue[static_cast<int>(event)];
}

std::ostream &operator<<(std::ostream &os, const tao::StateValue &state)
{
    os << stringify(state);
    return os;
}

/* *************************************************************************************************
Implementations of tao::GenericToasterState
************************************************************************************************* */

void tao::GenericToasterState::set_next_state(StateValue state)
{
    m_toaster->set_next_state(state);
}

void tao::GenericToasterState::unhandled_event(InternalEvent event)
{
    std::cout << "GenericToasterState::unhandled_event: " << stringify(event) << std::endl;
    switch (event)
    {
        case tao::InternalEvent::evt_stop:
            m_toaster->m_running = false;
            break;
        default:
            std::cout << "Event not handled at all" << std::endl;
            break;
    }
}

/* *************************************************************************************************
Implementations of tao::HeatingSuperState
************************************************************************************************* */
void tao::HeatingSuperState::on_entry()
{
    std::cout << "HeatingSuperState::on_entry" << std::endl;
    m_toaster->heater_on();
    m_toaster->arm_time_event(1);
}

void tao::HeatingSuperState::unhandled_event(InternalEvent event)
{
    std::cout << "HeatingSuperState::unhandled_event: " << stringify(event) << std::endl;
    switch (event)
    {
        case tao::InternalEvent::evt_door_open:
            m_toaster->m_door_status = Toaster::DoorStatus::opened;
            set_next_state(tao::StateValue::STATE_DOOR_OPEN);
            break;
        default:
            tao::GenericToasterState::unhandled_event(event);
            break;
    }
}

void tao::HeatingSuperState::process_internal_event(InternalEvent event)
{
    std::cout << "HeatingSuperState::process_internal_event: " << stringify(event) << std::endl;
    switch (event)
    {
        case tao::InternalEvent::evt_do_toasting:
            set_next_state(tao::StateValue::STATE_TOASTING);
            break;
        case tao::InternalEvent::evt_do_baking:
            set_next_state(tao::StateValue::STATE_BAKING);
            break;
        case tao::InternalEvent::evt_alarm_timeout:
            m_toaster->check_temp();
            break;
        default:
            tao::HeatingSuperState::unhandled_event(event);
            break;
    }
}

void tao::HeatingSuperState::on_exit()
{
    std::cout << "HeatingSuperState::on_exit" << std::endl;
    m_toaster->heater_off();
    m_toaster->disarm_time_event();
}

/* *************************************************************************************************
Implementations of tao::ToastingState
************************************************************************************************* */

void tao::ToastingState::on_entry(void)
{
    std::cout << "ToastingState::on_entry" << std::endl;
    m_toaster->arm_time_event(Toaster::ToastLevel::slightly_overcooked_toast);
}

void tao::ToastingState::process_internal_event(InternalEvent event)
{
    std::cout << "ToastingState::process_internal_event: " << stringify(event) << std::endl;
    switch (event)
    {
        case tao::InternalEvent::evt_alarm_timeout:
            set_next_state(tao::StateValue::STATE_HEATING);
            break;
        default:
            HeatingSuperState::unhandled_event(event);
            break;
    }
}

void tao::ToastingState::on_exit(void)
{
    std::cout << "ToastingState::on_exit" << std::endl;
    m_toaster->disarm_time_event();
}
/* *************************************************************************************************
Implementations of tao::BakingState
************************************************************************************************* */

void tao::BakingState::on_entry(void)
{
    std::cout << "BakingState::on_entry" << std::endl;
    m_toaster->set_target_temperature(15);
    m_toaster->arm_time_event(1);
}

void tao::BakingState::process_internal_event(InternalEvent event)
{
    std::cout << "BakingState::process_internal_event: " << stringify(event) << std::endl;
    switch (event)
    {
        case tao::InternalEvent::evt_alarm_timeout:
            if (m_toaster->temp_reached())
            {
                set_next_state(tao::StateValue::STATE_HEATING);
            }
            break;
        default:
            HeatingSuperState::unhandled_event(event);
            break;
    }
}

void tao::BakingState::on_exit(void)
{
    std::cout << "BakingState::on_exit" << std::endl;
    m_toaster->set_target_temperature(0);
    m_toaster->disarm_time_event();
}

/* *************************************************************************************************
Implementations of tao::DoorOpenState
************************************************************************************************* */
void tao::DoorOpenState::on_entry()
{
    std::cout << "DoorOpenState::on_entry" << std::endl;
    m_toaster->internal_lamp_on();
}

void tao::DoorOpenState::process_internal_event(InternalEvent event)
{
    std::cout << "DoorOpenState::process_internal_event: " << stringify(event) << std::endl;
    switch (event)
    {
        case tao::InternalEvent::evt_door_close:
            m_toaster->m_door_status = Toaster::DoorStatus::closed;
            set_next_state(tao::StateValue::STATE_HEATING);
            break;
        default:
            tao::GenericToasterState::unhandled_event(event);
            break;
    }
}

void tao::DoorOpenState::on_exit()
{
    std::cout << "DoorOpenState::on_exit" << std::endl;
    m_toaster->internal_lamp_off();
}


/* *************************************************************************************************
Implementations of ExternalEventWrapper
************************************************************************************************* */
tao::InternalEvent tao::ExternalEventWrapper::map_external_entity_to_internal_event(
    const ExternalEntityInfo &external_entity_event) const
{
    // Could be a std::map or a std::vector instead of a switch
    switch (external_entity_event.type())
    {
        case ExternalEntityInfo::Type::door_opened:
            return tao::InternalEvent::evt_door_open;
            break;
        case ExternalEntityInfo::Type::door_closed:
            return tao::InternalEvent::evt_door_close;
            break;
        default:
            return tao::InternalEvent::unknown;
    }
}

tao::InternalEvent tao::ExternalEventWrapper::map_external_to_internal_event()
{
    std::cout << "tao::ExternalEventWrapper::map_external_to_internal_event()" << std::endl;
    switch (m_type)
    {
        case tao::ExternalEventWrapper::EventType::external_entity_event:
            return map_external_entity_to_internal_event(boost::get<ExternalEntityInfo>(m_event));
            break;
        case tao::ExternalEventWrapper::EventType::internal_event:
            return boost::get<InternalEvent>(m_event);
            break;
        default:
            std::cout << "Discarding unknown external event for Toaster" << std::endl;
            return tao::InternalEvent::unknown;
            break;
    }
}

/* *************************************************************************************************
Implementations of Toaster
************************************************************************************************* */

void Toaster::set_next_state(tao::StateValue new_state)
{
    std::cout << "Toaster::set_next_state: " << stringify(new_state) << std::endl;
    m_next_state = new_state;
}

void Toaster::set_state(tao::StateValue new_state)
{
    std::cout << "Toaster::set_state: " << stringify(new_state) << std::endl;
    switch (new_state)
    {
        case tao::StateValue::STATE_HEATING:
            m_state = std::make_shared<tao::HeatingSuperState>(this);
            break;
        case tao::StateValue::STATE_TOASTING:
            m_state = std::make_shared<tao::ToastingState>(this);
            break;
        case tao::StateValue::STATE_BAKING:
            m_state = std::make_shared<tao::BakingState>(this);
            break;
        case tao::StateValue::STATE_DOOR_OPEN:
            m_state = std::make_shared<tao::DoorOpenState>(this);
            break;
        default:
            std::cout << "Attempt to set an invalid state" << std::endl;
            break;
    }
    m_next_state = tao::StateValue::UNKNOWN;
}

void Toaster::transition_state()
{
    if (m_next_state != tao::StateValue::UNKNOWN)
    {
        // @TODO Currently, on_exit() is called when switching from a superstate to one substate
        m_state->on_exit();
        set_state(m_next_state);
        m_state->on_entry();
    }
}

void Toaster::state_machine_iteration()
{
    std::cout << "Toaster::state_machine_iteration()" << std::endl;
    tao::InternalEvent curr_evt = m_queue->wait_and_pop()->map_external_to_internal_event();
    m_state->process_internal_event(curr_evt);
    transition_state();
}

void Toaster::state_machine_iteration(tao::InternalEvent evt)
{
    m_state->process_internal_event(evt);
    transition_state();
}

void Toaster::set_initial_state(tao::StateValue new_state)
{
    std::cout << "Toaster::set_initial_state: " << stringify(new_state) << std::endl;
    set_state(new_state);
    m_state->on_entry();
}

void Toaster::run()
{
    std::cout << "Toaster::run()" << std::endl;
    do
    {
        state_machine_iteration();
    } while (m_running);
}

void Toaster::start()
{
    std::cout << "Toaster::start()" << std::endl;
    m_running = true;
    m_thread  = std::thread(&Toaster::run, this);
}

void Toaster::stop()
{
    std::cout << "Toaster::stop()" << std::endl;
    if (!m_running)
        return;

    m_queue->put_prioritized(tao::ExternalEventWrapper(tao::InternalEvent::evt_stop));

    if (m_thread.joinable())
        m_thread.join();

    m_queue->clear();
}

/*
void Toaster::callback_external_entity_event(const ExternalEntityInfo &event)
{
   switch (event.type())
   {
   case ExternalEntityInfo::Type::door_opened:
      m_queue->put(tao::ExternalEventWrapper(event));
      break;
   case ExternalEntityInfo::Type::door_closed:
      m_queue->put_prioritized(tao::ExternalEventWrapper(event));
      break;
   default:
      std::cout << "Warning: Toaster trying to handle unexpected event: " << stringify(event) <<
std::endl; break;
   }
}
*/

void Toaster::heater_on()
{
    std::cout << "Toaster::heater_on()" << std::endl;
}

void Toaster::heater_off()
{
    std::cout << "Toaster::heater_off()" << std::endl;
}

void Toaster::internal_lamp_on()
{
    std::cout << "Toaster::internal_lamp_on()" << std::endl;
}

void Toaster::internal_lamp_off()
{
    std::cout << "Toaster::internal_lamp_off()" << std::endl;
}

void Toaster::arm_time_event(uint32_t time)
{
    std::cout << "Toaster::arm_time_event(uint32_t time)" << std::endl;
}

void Toaster::arm_time_event(ToastLevel level)
{
    std::cout << "Toaster::arm_time_event(ToastLevel level)" << std::endl;
}

void Toaster::disarm_time_event()
{
    std::cout << "Toaster::disarm_time_event()" << std::endl;
}

void Toaster::set_target_temperature(float temp)
{
    m_target_temp = temp;
    std::cout << "Toaster::set_target_temperature(float temp) - " << m_target_temp << std::endl;
}

float Toaster::check_temp()
{
    m_temp++;
    std::cout << "Toaster::check_temp() - " << m_temp << std::endl;
    return m_temp;
}

bool Toaster::temp_reached()
{
    std::cout << "Toaster::temp_reached()" << std::endl;
    return (check_temp() >= m_target_temp);
}