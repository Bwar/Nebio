/*******************************************************************************
 * Project:  Nebio
 * @file     CmdCollect.cpp
 * @brief    事件入口
 * @author   Bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/

#include "CmdCollect.hpp"

namespace nebio
{

CmdCollect::CmdCollect(int32 iCmd)
   : neb::Cmd(iCmd), m_dSessionTimeout(1200.0)
{
}

CmdCollect::~CmdCollect()
{
}

bool CmdCollect::Init()
{
    neb::CJsonObject oJsonConf = GetCustomConf();
    m_strChannelSummary = oJsonConf["analyse"]("channel_summary");
    m_strTagSummary = oJsonConf["analyse"]("tag_summary");
    m_strDirectAccess = oJsonConf["analyse"]("direct_access"); 
    oJsonConf["analyse"]["session_timeout"].Get("session_session", m_dSessionTimeout);
    return(true);
}

bool CmdCollect::AnyMessage(
        std::shared_ptr<neb::SocketChannel> pChannel, 
        const MsgHead& oMsgHead, const MsgBody& oMsgBody)
{
    Event oEvent;
    if (oEvent.ParseFromString(oMsgBody.data()))
    {
        LOG4_DEBUG("%s", oEvent.DebugString().c_str());
        std::string strSessionId = std::string("SessionSession-") + std::to_string(oEvent.app_id()) + "-" + oEvent.session_id();
        auto pSession = GetSession(strSessionId);
        if (pSession == nullptr)
        {
            pSession = MakeSharedSession("nebio::SessionSession", strSessionId, m_dSessionTimeout);
        }
        if (pSession == nullptr)
        {
            return(false);
        }

        (std::dynamic_pointer_cast<SessionSession>(pSession))->AddEvent(oEvent);
        return(true);
    }
    else
    {
        LOG4_ERROR("nebio::Event failed to parse MsgBody.data()!");
        return(false);
    }
}

}

