#include "Events.hpp"

const std::string& stringify(const ExternalEntityInfo& event)
{
    static const std::map<ExternalEntityInfo::Type, std::string> stringifier_map{
        {ExternalEntityInfo::Type::unknown, "ExternalEntityInfo::Type::unknown"},
        {ExternalEntityInfo::Type::trigger_something,
         "ExternalEntityInfo::Type::trigger_something"},
        {ExternalEntityInfo::Type::door_opened, "ExternalEntityInfo::Type::door_opened"},
        {ExternalEntityInfo::Type::door_closed, "ExternalEntityInfo::Type::door_closed"}};
    auto result_iterator = stringifier_map.find(event.type());
    if (result_iterator != stringifier_map.end())
    {
        return result_iterator->second;
    }
    return stringifier_map.at(ExternalEntityInfo::Type::unknown);
}
std::ostream& operator<<(std::ostream& os, const ExternalEntityInfo& orchestrator_info)
{
    os << stringify(orchestrator_info);
    return os;
}