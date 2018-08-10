/*******************************************************************************
 * Project:  Nebio
 * @file     CmdPageIv.cpp
 * @brief    页面入口
 * @author   Bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/

#include <sstream>
#include "CmdPageIv.hpp"

namespace nebio
{

CmdPageIv::CmdPageIv(int32 iCmd)
   : neb::Cmd(iCmd), m_dSessionTimeout(10.0)
{
}

CmdPageIv::~CmdPageIv()
{
}

bool CmdPageIv::Init()
{
    neb::CJsonObject oJsonConf = GetCustomConf();
    m_strChannelSummary = oJsonConf["analyse"]("channel_summary");
    m_strTagSummary = oJsonConf["analyse"]("tag_summary");
    oJsonConf["analyse"]["session_timeout"].Get("session_page_iv", m_dSessionTimeout);
    return(true);
}

bool CmdPageIv::AnyMessage(
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

bool CmdPageIv::Stat(const std::string& strChannel, const std::string& strTag, const Event& oEvent)
{
    if (strChannel.length() == 0 || strTag.length() == 0)
    {
        return(true);
    }
    
    std::ostringstream oss;
    oss << "SessionPageIv-" << oEvent.app_id() << "-" << strChannel << "-" << strTag << "-" << oEvent.page();
    std::string strSessionId = oss.str();
    auto pSession = GetSession(strSessionId);
    if (pSession == nullptr)
    {
        pSession = MakeSharedSession("nebio::SessionPageIv", strSessionId, strChannel, strTag, m_dSessionTimeout);
    }
    if (pSession == nullptr)
    {
        return(false);
    }

    (std::dynamic_pointer_cast<SessionPageIv>(pSession))->AddEvent(oEvent);
    return(true);
}

}
