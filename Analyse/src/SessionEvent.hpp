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
#include "AnalyseSession.hpp"

namespace nebio
{

class SessionEvent : public AnalyseSession,
    public neb::DynamicCreator<SessionEvent, std::string, std::string, std::string, ev_tstamp>
{
public:
    SessionEvent(const std::string& strSessionId, const std::string& strChannel, const std::string& strTag,
        ev_tstamp dSessionTimeout = 10.0);
    virtual ~SessionEvent();

    virtual neb::E_CMD_STATUS Timeout();

    void AddEvent(const Event& oEvent);

protected:
    void SendResult();

private:
    uint32 m_uiAppId;
    std::string m_strChannel;
    std::string m_strTag;
    std::string m_strEventId;
    std::string m_strEventType;
     
    uint32 m_uiPv;
    uint32 m_uiVv;
    uint64 m_ullEventLength;
    std::unordered_set<std::string> m_setUserId;
};

} // namespace nebio

#endif // SESSIONEVNET_HPP_

