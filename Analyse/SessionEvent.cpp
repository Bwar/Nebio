/*******************************************************************************
 * Project:  Nebio
 * @file     SessionEvent.cpp
 * @brief    事件统计
 * @author   bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/

#include "SessionEvent.hpp"

SessionEvent::SessionEvent(const std::string& strSessionId, ev_tstamp dSessionTimeout)
    : neb::Session(strSessionId, dSessionTimeout, "nebio::SessionEvent")
{
}

SessionEvent::~SessionEvent()
{
}

void SessionEvent::AddEvent(const neb::CJsonObject& oEvent)
{
    if (m_strEventId.size() == 0)
    {
        m_strEventId = oEvent("event_id");
        m_strEventType = oEvent("event_type");
        m_strPage = oEvent("page"); // TODO parse page url
    }
    
}

