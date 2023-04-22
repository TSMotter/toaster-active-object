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
        case tao::InternalEvent::evt_door_close:
            m_toaster->m_door_status = Toaster::DoorStatus::closed;
            set_next_state(tao::StateValue::STATE_HEATING);
            break;
        case tao::InternalEvent::evt_door_open:
            m_toaster->m_door_status = Toaster::DoorStatus::opened;
            set_next_state(tao::StateValue::STATE_DOOR_OPEN);
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
}

void tao::HeatingSuperState::unhandled_event(InternalEvent event)
{
    std::cout << "HeatingSuperState::unhandled_event: " << stringify(event) << std::endl;
    tao::GenericToasterState::unhandled_event(event);
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
        default:
            tao::GenericToasterState::unhandled_event(event);
            break;
    }
}

void tao::HeatingSuperState::on_exit()
{
    std::cout << "HeatingSuperState::on_exit" << std::endl;
    m_toaster->heater_off();
}

/* *************************************************************************************************
Implementations of tao::ToastingState
************************************************************************************************* */

void tao::ToastingState::on_entry(void)
{
    std::cout << "ToastingState::on_entry" << std::endl;
    m_toaster->heater_on();  /* TODO: This might be removed if Issue#2 is fixed */
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
    m_toaster->heater_off();  /* TODO: This might be removed if Issue#2 is fixed */
    m_toaster->disarm_time_event();
}

/* *************************************************************************************************
Implementations of tao::BakingState
************************************************************************************************* */

void tao::BakingState::on_entry(void)
{
    std::cout << "BakingState::on_entry" << std::endl;
    m_toaster->heater_on();                 /* TODO: This might be removed if Issue#2 is fixed */
    m_toaster->set_target_temperature(50);  /* TODO: Issue#4 */
}

void tao::BakingState::process_internal_event(InternalEvent event)
{
    std::cout << "BakingState::process_internal_event: " << stringify(event) << std::endl;
    switch (event)
    {
        case tao::InternalEvent::evt_alarm_timeout:
            set_next_state(tao::StateValue::STATE_HEATING);
            break;
        case tao::InternalEvent::evt_target_temp_reached:
            /* TODO: Issue#3 */
            m_toaster->arm_time_event(10000);
            break;
        default:
            HeatingSuperState::unhandled_event(event);
            break;
    }
}

void tao::BakingState::on_exit(void)
{
    std::cout << "BakingState::on_exit" << std::endl;
    m_toaster->unset_target_temperature();
    m_toaster->heater_off();  /* TODO: This might be removed if Issue#2 is fixed */
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
    tao::GenericToasterState::unhandled_event(event);
}

void tao::DoorOpenState::on_exit()
{
    std::cout << "DoorOpenState::on_exit" << std::endl;
    m_toaster->internal_lamp_off();
}


/* *************************************************************************************************
Implementations of IncomingEventWrapper
************************************************************************************************* */
tao::InternalEvent tao::IncomingEventWrapper::map_external_entity_event_to_internal_event(
    const ExternalEntityEvent &evt) const
{
    switch (evt.which())
    {
        case ExternalEntityEvtType::stop_request:
            return tao::InternalEvent::evt_stop;
            break;
        case ExternalEntityEvtType::toast_request:
            return tao::InternalEvent::evt_do_toasting;
            break;
        case ExternalEntityEvtType::bake_request:
            return tao::InternalEvent::evt_do_baking;
            break;
        case ExternalEntityEvtType::opening_door:
            return tao::InternalEvent::evt_door_open;
            break;
        case ExternalEntityEvtType::closing_door:
            return tao::InternalEvent::evt_door_close;
            break;
        default:
            return tao::InternalEvent::unknown;
    }
}

tao::InternalEvent tao::IncomingEventWrapper::map_incoming_event_to_internal_event()
{
    std::cout << "tao::IncomingEventWrapper::map_incoming_event_to_internal_event()" << std::endl;
    switch (m_type)
    {
        case tao::IncomingEventWrapper::EventType::external_entity_event:
            return map_external_entity_event_to_internal_event(
                boost::get<ExternalEntityEvent>(m_event));
            break;
        case tao::IncomingEventWrapper::EventType::internal_event:
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
        /* TODO: Issue#2 */
        m_state->on_exit();
        set_state(m_next_state);
        m_state->on_entry();
    }
}

void Toaster::state_machine_iteration()
{
    std::cout << "Toaster::state_machine_iteration()" << std::endl;
    tao::InternalEvent curr_evt = m_queue->wait_and_pop()->map_incoming_event_to_internal_event();
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

    m_queue->put_prioritized(tao::IncomingEventWrapper(tao::InternalEvent::evt_stop));

    if (m_thread.joinable())
        m_thread.join();

    m_queue->clear();
}

void Toaster::put_external_entity_event(const ExternalEntityEvent &evt)
{
    switch (evt.which())
    {
        case ExternalEntityEvtType::stop_request:
        case ExternalEntityEvtType::toast_request:
        case ExternalEntityEvtType::bake_request:
        case ExternalEntityEvtType::opening_door:
        case ExternalEntityEvtType::closing_door:
            // m_queue->put_prioritized(tao::IncomingEventWrapper(evt));
            // m_queue->put(tao::IncomingEventWrapper(evt));
            generic_event_putter(evt);
            break;
        default:
            std::cout << "Warning: Received unhandled event from external entity: "
                      << stringify(evt) << std::endl;
            break;
    }
}

void Toaster::heater_on()
{
    std::cout << "Toaster::heater_on()" << std::endl;
    m_heater.turn_on();
}

void Toaster::heater_off()
{
    std::cout << "Toaster::heater_off()" << std::endl;
    m_heater.turn_off();
}

void Toaster::internal_lamp_on()
{
    std::cout << "Toaster::internal_lamp_on()" << std::endl;
}

void Toaster::internal_lamp_off()
{
    std::cout << "Toaster::internal_lamp_off()" << std::endl;
}

void Toaster::arm_time_event(long time)
{
    std::cout << "Toaster::arm_time_event(long time)" << std::endl;
    m_timer.start(time);
}

void Toaster::arm_time_event(ToastLevel level)
{
    std::cout << "Toaster::arm_time_event(ToastLevel level)" << std::endl;
    long period = static_cast<long>(level) * 2000;  // Arbitrary hardcoded value
    m_timer.start(period);
}

void Toaster::disarm_time_event()
{
    std::cout << "Toaster::disarm_time_event()" << std::endl;
    m_timer.stop();
}

void Toaster::set_target_temperature(float temp)
{
    m_temp_sensor.set_target_temp(temp);
    std::cout << "Toaster::set_target_temperature(float temp) - " << temp << std::endl;
}

void Toaster::unset_target_temperature()
{
    m_temp_sensor.unset_target_temp();
    std::cout << "Toaster::unset_target_temperature()" << std::endl;
}