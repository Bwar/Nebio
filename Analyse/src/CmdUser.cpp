/*******************************************************************************
 * Project:  Nebio
 * @file     CmdUser.cpp
 * @brief    事件入口
 * @author   Bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/

#include <sstream>
#include "UnixTime.hpp"
#include "CmdUser.hpp"

namespace nebio
{

CmdUser::CmdUser(int32 iCmd)
   : neb::Cmd(iCmd)
{
}

CmdUser::~CmdUser()
{
}

bool CmdUser::Init()
{
    neb::CJsonObject oJsonConf = GetCustomConf();
    m_strChannelSummary = oJsonConf["analyse"]("channel_summary");
    m_strTagSummary = oJsonConf["analyse"]("tag_summary");
    return(true);
}

bool CmdUser::AnyMessage(
        std::shared_ptr<neb::SocketChannel> pChannel, 
        const MsgHead& oMsgHead, const MsgBody& oMsgBody)
{
    Event oEvent;
    if (oEvent.ParseFromString(oMsgBody.data()))
    {
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

bool CmdUser::Stat(const std::string& strChannel, const std::string& strTag, const Event& oEvent)
{
    if (strChannel.length() == 0 || strTag.length() == 0)
    {
        return(true);
    }
    
    std::ostringstream oss;
    oss << oEvent.app_id() << "-" << strChannel << "-" << strTag << "-" << oEvent.event_id();
    std::string strSessionId = oss.str();
    auto pSession = GetSession(strSessionId, "nebio::SessionUser");
    if (pSession == nullptr)
    {
        uint64 ullStatDate = neb::GetBeginTimeOfTheDay(time(NULL));
        pSession = MakeSharedSession("nebio::SessionUser", strSessionId, strChannel, strTag, ullStatDate, 10.0);
    }
    if (pSession == nullptr)
    {
        return(false);
    }

    (std::dynamic_pointer_cast<SessionUser>(pSession))->AddEvent(oEvent);
    return(true);
}

}
