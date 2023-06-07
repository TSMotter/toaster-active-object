#include <iostream>

#include "ActiveObject.hpp"

// *****************************************************************************

const std::string &ao::stringify(ao::InternalEvent event)
{
    static const std::string unknown_internal_event = "invalid stringify InternalEvent";
    auto                     it = stringfier_InternalEvent.find(static_cast<int>(event));
    if (it != stringfier_InternalEvent.end())
    {
        return it->second;
    }
    return unknown_internal_event;
}

std::ostream &operator<<(std::ostream &os, const ao::InternalEvent &event)
{
    os << stringify(event);
    return os;
}

const std::string &ao::stringify(ao::StateValue event)
{
    return stringfier_StateValue[static_cast<int>(event)];
}

std::ostream &operator<<(std::ostream &os, const ao::StateValue &state)
{
    os << stringify(state);
    return os;
}

// *****************************************************************************

void ao::GenericState::set_next_state(StateValue state)
{
    m_ett->set_next_state(state);
}

void ao::GenericState::unhandled_event(InternalEvent event)
{
    std::cout << "GenericState::unhandled_event: " << stringify(event) << std::endl;
    switch (event)
    {
        case InternalEvent::intEvt4:
            // Do something
            set_next_state(StateValue::STATE1);
            break;
        default:
            break;
    }
}

// *****************************************************************************

void ao::Superstate1::unhandled_event(InternalEvent event)
{
    std::cout << "Superstate1::unhandled_event: " << stringify(event) << std::endl;
    switch (event)
    {
        case InternalEvent::intEvt3:
            // Do something
            set_next_state(StateValue::STATE2);
            break;
        default:
            ao::GenericState::unhandled_event(event);
            break;
    }
}

// *****************************************************************************

void ao::NormalState1::on_entry(void)
{
    std::cout << "NormalState1::on_entry" << std::endl;
}

void ao::NormalState1::process_internal_event(InternalEvent event)
{
    std::cout << "NormalState1::process_internal_event: " << stringify(event) << std::endl;
    switch (event)
    {
        case InternalEvent::intEvt2:
            // Do something
            set_next_state(StateValue::STATE2);
            break;
        default:
            Superstate1::unhandled_event(event);
            break;
    }
}

// *****************************************************************************

void ao::NormalState2::on_entry(void)
{
    std::cout << "NormalState2::on_entry" << std::endl;
}

void ao::NormalState2::process_internal_event(InternalEvent event)
{
    std::cout << "NormalState2::process_internal_event: " << stringify(event) << std::endl;
    switch (event)
    {
        case InternalEvent::intEvt1:
            // Do something
            set_next_state(StateValue::STATE1);
            break;
        default:
            Superstate1::unhandled_event(event);
            break;
    }
}

void ao::NormalState2::on_exit(void)
{
    std::cout << "NormalState2::on_exit" << std::endl;
}

// *****************************************************************************

void Entity::set_next_state(ao::StateValue new_state)
{
    std::cout << "Entity::set_next_state: " << stringify(new_state) << std::endl;
    m_next_state = new_state;
}

void Entity::set_state(ao::StateValue new_state)
{
    switch (new_state)
    {
        case ao::StateValue::STATE1:
            m_state = std::make_shared<ao::NormalState1>(this);
            break;
        case ao::StateValue::STATE2:
            m_state = std::make_shared<ao::NormalState2>(this);
            break;
        default:
            std::cout << "Attempt to set an invalid state" << std::endl;
            break;
    }
    std::cout << "Entity::set_state: " << stringify(new_state) << std::endl;
    m_next_state = ao::StateValue::UNKNOWN;
}

void Entity::transition_state()
{
    if (m_next_state != ao::StateValue::UNKNOWN)
    {
        m_state->on_exit();
        set_state(m_next_state);
        m_state->on_entry();
    }
}

void Entity::state_machine_iteration(ao::InternalEvent evt)
{
    // std::cout << "Entity::state_machine_iteration: " << stringify(m_state) <<
    // std::endl;
    m_state->process_internal_event(evt);
    transition_state();
}

void Entity::set_initial_state(ao::StateValue new_state)
{
    std::cout << "Entity::set_initial_state: " << stringify(new_state) << std::endl;
    set_state(new_state);
    m_state->on_entry();
}