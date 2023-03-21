#ifndef __EVENTS__
#define __EVENTS__

#include <string>
#include <map>
#include <iostream>

//#include "boost/signals2.hpp"

struct ExternalEntityInfo
{
    enum class Type
    {
        unknown,
        trigger_something,
        door_opened,
        door_closed,
        stopped,
    };
    ExternalEntityInfo(const Type& event_type) : m_type{event_type}
    {
    }
    Type type() const
    {
        return m_type;
    }
    void type(Type type)
    {
        m_type = type;
    };
    std::string info() const
    {
        std::string stringified_type{};
        switch (type())
        {
            case Type::unknown:
                stringified_type = "ExternalEntityInfo::Type::unknown";
                break;
            case Type::trigger_something:
                stringified_type = "ExternalEntityInfo::Type::trigger_something";
                break;
            case Type::door_opened:
                stringified_type = "ExternalEntityInfo::Type::door_opened";
                break;
            case Type::door_closed:
                stringified_type = "ExternalEntityInfo::Type::door_closed";
                break;
            default:
                stringified_type = "Unhandled ExternalEntityInfo enum value";
                break;
        }
        return stringified_type;
    }

   private:
    Type m_type = {Type::unknown};
};
std::ostream&      operator<<(std::ostream& os, const ExternalEntityInfo& orchestrator_info);
const std::string& stringify(const ExternalEntityInfo& event);


#endif