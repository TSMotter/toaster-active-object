#include "Events.hpp"

const std::string& stringify(const ExternalEntityEvent& event)
{
    static const std::map<ExternalEntityEvtType, std::string> stringifier_map{
        {ExternalEntityEvtType::unknown, "ExternalEntityEvtType::unknown"},
        {ExternalEntityEvtType::trigger_something, "ExternalEntityEvtType::trigger_something"},
        {ExternalEntityEvtType::door_opened, "ExternalEntityEvtType::door_opened"},
        {ExternalEntityEvtType::door_closed, "ExternalEntityEvtType::door_closed"}};
    auto result_iterator = stringifier_map.find(event.which());
    if (result_iterator != stringifier_map.end())
    {
        return result_iterator->second;
    }
    return stringifier_map.at(ExternalEntityEvtType::unknown);
}
std::ostream& operator<<(std::ostream& os, const ExternalEntityEvent& evt)
{
    os << stringify(evt);
    return os;
}