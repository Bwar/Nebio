/*******************************************************************************
 * Project:  Nebio
 * @file     SessionEvent.hpp
 * @brief    事件统计
 * @author   bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/
#ifndef SESSIONEVENT_HPP_
#define SESSIONEVENT_HPP_

#include <string>
#include <unordered_set>
#include <unordered_map>
#include "actor/session/Session.hpp"

namespace nebio
{

class SessionEvent : public neb::Session,
    public DynamicCreator<SessionEvent, std::string, std::string, ev_tstamp>
{
public:
    SessionEvent(const std::string& strSessionId, ev_tstamp dSessionTimeout = 10.0);
    virtual ~SessionEvent();

    void AddEvent(const neb::CJsonObject& oEvent);

private:
    std::string m_strEventId;
    std::string m_strEventType;
    std::string m_strPage;
    std::unordered_set<std::string> m_setUserId;
    std::unordered_set<std::string> m_setIp;
    std::unordered_map<std::string, std::unordered_set<std::string> > m_mapChannelUserId;
    std::unordered_map<std::string, std::unordered_set<std::string> > m_mapChannelIp;
};

} // namespace nebio

#endif // SESSIONEVNET_HPP_

