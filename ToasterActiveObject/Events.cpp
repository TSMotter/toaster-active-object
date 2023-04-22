#include "Events.hpp"

/* *************************************************************************************************
ExternalEntity
************************************************************************************************* */
const std::string& stringify(const ExternalEntityEvent& event)
{
    auto result_iterator = stringifier_map_external_entity_evt.find(event.which());
    if (result_iterator != stringifier_map_external_entity_evt.end())
    {
        return result_iterator->second;
    }
    return stringifier_map_external_entity_evt.at(ExternalEntityEvtType::unknown);
}
std::ostream& operator<<(std::ostream& os, const ExternalEntityEvent& evt)
{
    os << stringify(evt);
    return os;
}

/* *************************************************************************************************
TempSensor
************************************************************************************************* */
const std::string& stringify(const TempSensorEvent& event)
{
    auto result_iterator = stringifier_map_temp_sensor_evt.find(event.which());
    if (result_iterator != stringifier_map_temp_sensor_evt.end())
    {
        return result_iterator->second;
    }
    return stringifier_map_temp_sensor_evt.at(TempSensorEvtType::unknown);
}
std::ostream& operator<<(std::ostream& os, const TempSensorEvent& evt)
{
    os << stringify(evt);
    return os;
}