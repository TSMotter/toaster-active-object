#ifndef __EVENTS__
#define __EVENTS__

#include <string>
#include <map>
#include <iostream>

//#include "boost/signals2.hpp"

enum class ExternalEntityEvtType
{
    unknown,
    stop_request,
    toast_request,
    bake_request,
    opening_door,
    closing_door,
};

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


#endif