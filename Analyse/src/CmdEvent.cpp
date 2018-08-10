/*******************************************************************************
 * Project:  Nebio
 * @file     CmdEvent.cpp
 * @brief    事件入口
 * @author   Bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/

#include <sstream>
#include "CmdEvent.hpp"

namespace nebio
{

CmdEvent::CmdEvent(int32 iCmd)
   : neb::Cmd(iCmd), m_dSessionTimeout(10.0)
{
}

CmdEvent::~CmdEvent()
{
}

bool CmdEvent::Init()
{
    neb::CJsonObject oJsonConf = GetCustomConf();
    m_strChannelSummary = oJsonConf["analyse"]("channel_summary");
    m_strTagSummary = oJsonConf["analyse"]("tag_summary");
    oJsonConf["analyse"]["session_timeout"].Get("session_event", m_dSessionTimeout);
    return(true);
}

bool CmdEvent::AnyMessage(
        std::shared_ptr<neb::SocketChannel> pChannel, 
        const MsgHead& oMsgHead, const MsgBody& oMsgBody)
{
    Event oEvent;
    if (oEvent.ParseFromString(oMsgBody.data()))
    {
        LOG4_DEBUG("%s", oEvent.DebugString().c_str());
        Stat(m_strChannelSummary, m_strTagSummary, oEvent);
        Stat(oEvent.referer(), oEvent.tag(), oEvent);
        Stat(oEvent.referer(), m_strTagSummary, oEvent);
        return(true);
    }
    else
    {
        LOG4_ERROR("nebio::Event failed to parse MsgBody.data()!");
        return(false);
    }
}

bool CmdEvent::Stat(const std::string& strChannel, const std::string& strTag, const Event& oEvent)
{
    if (strChannel.length() == 0 || strTag.length() == 0)
    {
        return(true);
    }
    
    std::ostringstream oss;
    oss << "SessionEvent-" << oEvent.app_id() << "-" << strChannel << "-" << strTag << "-" << oEvent.event_id();
    std::string strSessionId = oss.str();
    auto pSession = GetSession(strSessionId);
    if (pSession == nullptr)
    {
        pSession = MakeSharedSession("nebio::SessionEvent", strSessionId, strChannel, strTag, m_dSessionTimeout);
    }
    if (pSession == nullptr)
    {
        return(false);
    }

    std::shared_ptr<SessionEvent> pSessionSession = std::dynamic_pointer_cast<SessionEvent>(pSession);
    pSessionSession->AddEvent(oEvent);
    return(true);
}

}
