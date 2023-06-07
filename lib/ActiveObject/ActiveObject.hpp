#ifndef __ACTIVEOBJECT__
#define __ACTIVEOBJECT__

#include <iostream>
#include <map>
#include <memory>
#include <vector>

class Entity;

namespace ao
{
enum class InternalEvent
{
    unknown,
    intEvt1,
    intEvt2,
    intEvt3,
    intEvt4,
    intEvt5,
};
const std::string                      &stringify(InternalEvent event);
static const std::map<int, std::string> stringfier_InternalEvent{
    {static_cast<int>(InternalEvent::unknown), "unknown"},
    {static_cast<int>(InternalEvent::intEvt1), "intEvt1"},
    {static_cast<int>(InternalEvent::intEvt2), "intEvt2"},
    {static_cast<int>(InternalEvent::intEvt3), "intEvt3"},
    {static_cast<int>(InternalEvent::intEvt4), "intEvt4"},
    {static_cast<int>(InternalEvent::intEvt5), "intEvt5"}};

enum class StateValue
{
    UNKNOWN = 0,
    STATE1,
    STATE2,
    STATE3,
};
const std::string                    &stringify(StateValue state);
static const std::vector<std::string> stringfier_StateValue{"UNKNOWN", "STATE1", "STATE2",
                                                            "STATE3"};

class GenericState
{
   protected:
    Entity *m_ett;

   private:
    StateValue m_state;

   public:
    GenericState(Entity *ett, StateValue state = StateValue::UNKNOWN) : m_ett{ett}, m_state{state}
    {
    }

    virtual ~GenericState() = default;

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

class Superstate1 : public GenericState
{
   public:
    virtual ~Superstate1() = default;
    Superstate1(Entity *ett, StateValue state) : GenericState(ett, state)
    {
    }
    virtual void unhandled_event(InternalEvent event) override;
    virtual void process_internal_event(InternalEvent event) = 0;
};

class NormalState1 : public Superstate1
{
   public:
    virtual ~NormalState1()
    {
    }
    NormalState1(Entity *ett) : Superstate1(ett, StateValue::STATE1)
    {
    }
    virtual void on_entry() override;
    virtual void process_internal_event(InternalEvent event) override;
};
class NormalState2 : public Superstate1
{
   public:
    virtual ~NormalState2()
    {
    }
    NormalState2(Entity *ett) : Superstate1(ett, StateValue::STATE2)
    {
    }
    virtual void on_entry() override;
    virtual void process_internal_event(InternalEvent event) override;
    virtual void on_exit() override;
};
}  // namespace ao

class Entity
{
   public:
    bool                              m_running{false};
    std::shared_ptr<ao::GenericState> m_state;
    ao::StateValue                    m_next_state{ao::StateValue::UNKNOWN};

   public:
    Entity(float x, float y) : m_x{x}, m_y{y}
    {
        // load_configs(config);
        set_initial_state(ao::StateValue::STATE1);
    }
    ~Entity()
    {
    }
    void set_next_state(ao::StateValue new_state);
    void set_state(ao::StateValue new_state);
    void transition_state();
    void state_machine_iteration(ao::InternalEvent evt);
    void set_initial_state(ao::StateValue new_state);

   private:
    float m_x;
    float m_y;
};

#endif