/*******************************************************************************
 * Project:  Nebio
 * @file     CmdEventIv.cpp
 * @brief    事件入口
 * @author   Bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/

#include <sstream>
#include "CmdEventIv.hpp"
#include "UnixTime.hpp"

namespace nebio
{

CmdEventIv::CmdEventIv(int32 iCmd)
   : neb::Cmd(iCmd), m_dSessionTimeout(10.0)
{
}

CmdEventIv::~CmdEventIv()
{
}

bool CmdEventIv::Init()
{
    neb::CJsonObject oJsonConf = GetCustomConf();
    m_strChannelSummary = oJsonConf["analyse"]("channel_summary");
    m_strTagSummary = oJsonConf["analyse"]("tag_summary");
    oJsonConf["analyse"]["session_timeout"].Get("session_event_iv", m_dSessionTimeout);
    return(true);
}

bool CmdEventIv::AnyMessage(
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

bool CmdEventIv::Stat(const std::string& strChannel, const std::string& strTag, const Event& oEvent)
{
    if (strChannel.length() == 0 || strTag.length() == 0 || oEvent.event_id().length() == 0)
    {
        return(true);
    }
    
    std::ostringstream oss;
    oss << "SessionEventIv-" << oEvent.app_id() << "-" << strChannel << "-" << strTag << "-" << oEvent.event_id();
    std::string strSessionId = oss.str();
    auto pSession = GetSession(strSessionId);
    if (pSession == nullptr)
    {
        m_uiDate = std::stoul(neb::time_t2TimeStr((time_t)GetNowTime(), "%Y%m%d"));
        pSession = MakeSharedSession("nebio::SessionEventIv", strSessionId, strChannel, strTag, m_uiDate, m_dSessionTimeout);
    }
    if (pSession == nullptr)
    {
        return(false);
    }

    std::shared_ptr<SessionEventIv> pSessionSession = std::dynamic_pointer_cast<SessionEventIv>(pSession);
    pSessionSession->AddEvent(oEvent);
    return(true);
}

}
