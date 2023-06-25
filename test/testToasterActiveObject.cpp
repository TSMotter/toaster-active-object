#include <gtest/gtest.h>
#include <thread>
#include <vector>

#include "ToasterActiveObject.hpp"

// Fixture definition
class ToasterActiveObjectFixture : public ::testing::Test
{
   protected:
    ToasterActiveObjectFixture()
        : m_toaster{std::make_shared<Toaster>(std::make_shared<DemoObjects::HeaterDemo>(),
                                              std::make_shared<DemoObjects::TempSensorDemo>())}
    {
        // You can do set-up work for each test here.
    }

    ~ToasterActiveObjectFixture()
    {
        // You can do clean-up work that doesn't throw exceptions here.
        m_toaster->stop();
    }

    void external_event_putter(const ExternalEntityEvent &evt)
    {
        m_toaster->put_external_entity_event(evt);
        m_toaster->run();
    }


    testing::AssertionResult assertState(tao::StateValue ref)
    {
        if (m_toaster->m_state->type() == ref)
        {
            return testing::AssertionSuccess();
        }
        else
        {
            return testing::AssertionFailure()
                   << "The current value: " << stringify(m_toaster->m_state->type())
                   << " is different from the expected value: " << stringify(ref);
        }
    }

    std::shared_ptr<Toaster> m_toaster;
};

TEST_F(ToasterActiveObjectFixture, TestDefaultStart)
{
    m_toaster->run();
    ASSERT_TRUE(assertState(tao::StateValue::STATE_HEATING));
}

TEST_F(ToasterActiveObjectFixture, TestEventToast)
{
    external_event_putter(ExternalEntityEvtType::toast_request);
    ASSERT_TRUE(assertState(tao::StateValue::STATE_TOASTING));
}

TEST_F(ToasterActiveObjectFixture, TestEventToastCheckTemp)
{
    external_event_putter(ExternalEntityEvtType::toast_request);
    ASSERT_TRUE(assertState(tao::StateValue::STATE_TOASTING));
}

TEST_F(ToasterActiveObjectFixture, TestEventBake)
{
    external_event_putter(ExternalEntityEvtType::bake_request);
    ASSERT_TRUE(assertState(tao::StateValue::STATE_BAKING));
}

TEST_F(ToasterActiveObjectFixture, TestEventOpeningDoor)
{
    external_event_putter(ExternalEntityEvtType::opening_door);
    ASSERT_TRUE(assertState(tao::StateValue::STATE_DOOR_OPEN));
}

TEST_F(ToasterActiveObjectFixture, TestEventToastInterruptedBecauseOfOpenDoor)
{
    external_event_putter(ExternalEntityEvtType::toast_request);
    external_event_putter(ExternalEntityEvtType::opening_door);
    ASSERT_TRUE(assertState(tao::StateValue::STATE_DOOR_OPEN));
}
