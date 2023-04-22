#ifndef __EVENTS__
#define __EVENTS__

#include <string>
#include <map>
#include <iostream>

/* *************************************************************************************************
ExternalEntity
************************************************************************************************* */
enum class ExternalEntityEvtType
{
    unknown,
    stop_request,
    toast_request,
    bake_request,
    opening_door,
    closing_door,
};
static const std::map<ExternalEntityEvtType, std::string> stringifier_map_external_entity_evt{
    {ExternalEntityEvtType::unknown, "ExternalEntityEvtType::unknown"},
    {ExternalEntityEvtType::stop_request, "ExternalEntityEvtType::stop_request"},
    {ExternalEntityEvtType::toast_request, "ExternalEntityEvtType::toast_request"},
    {ExternalEntityEvtType::bake_request, "ExternalEntityEvtType::bake_request"},
    {ExternalEntityEvtType::opening_door, "ExternalEntityEvtType::opening_door"},
    {ExternalEntityEvtType::closing_door, "ExternalEntityEvtType::closing_door"}};

struct ExternalEntityEvent
{
    ExternalEntityEvent(const ExternalEntityEvtType event) : m_event{event}
    {
    }
    const ExternalEntityEvtType& which() const
    {
        return m_event;
    }

   public:
    ExternalEntityEvtType m_event;
};

std::ostream&      operator<<(std::ostream& os, const ExternalEntityEvent& orchestrator_info);
const std::string& stringify(const ExternalEntityEvent& event);

/* *************************************************************************************************
TempSensor
************************************************************************************************* */
enum class TempSensorEvtType
{
    unknown,
    target_temp_reached
};
static const std::map<TempSensorEvtType, std::string> stringifier_map_temp_sensor_evt{
    {TempSensorEvtType::unknown, "TempSensorEvtType::unknown"},
    {TempSensorEvtType::target_temp_reached, "TempSensorEvtType::target_temp_reached"}};

struct TempSensorEvent
{
    TempSensorEvent(const TempSensorEvtType event) : m_event{event}
    {
    }
    const TempSensorEvtType& which() const
    {
        return m_event;
    }

   public:
    TempSensorEvtType m_event;
};

std::ostream&      operator<<(std::ostream& os, const TempSensorEvent& orchestrator_info);
const std::string& stringify(const TempSensorEvent& event);

#endif