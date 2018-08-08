/*******************************************************************************
 * Project:  Nebio
 * @file     CmdUserIv.cpp
 * @brief    事件入口
 * @author   Bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/

#include <sstream>
#include "UnixTime.hpp"
#include "CmdUserIv.hpp"

namespace nebio
{

CmdUserIv::CmdUserIv(int32 iCmd)
   : neb::Cmd(iCmd)
{
}

CmdUserIv::~CmdUserIv()
{
}

bool CmdUserIv::Init()
{
    neb::CJsonObject oJsonConf = GetCustomConf();
    m_strChannelSummary = oJsonConf["analyse"]("channel_summary");
    m_strTagSummary = oJsonConf["analyse"]("tag_summary");
    return(true);
}

bool CmdUserIv::AnyMessage(
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

bool CmdUserIv::Stat(const std::string& strChannel, const std::string& strTag, const Event& oEvent)
{
    if (strChannel.length() == 0 || strTag.length() == 0)
    {
        return(true);
    }
    
    std::ostringstream oss;
    oss << "SessionUserIv-" + oEvent.app_id() << "-" << strChannel << "-" << strTag;
    std::string strSessionId = oss.str();
    auto pSession = GetSession(strSessionId);
    if (pSession == nullptr)
    {
        uint64 ullStatDate = neb::GetBeginTimeOfTheDay(time(NULL));
        pSession = MakeSharedSession("nebio::SessionUserIv", strSessionId, strChannel, strTag, ullStatDate, 10.0);
    }
    if (pSession == nullptr)
    {
        return(false);
    }

    (std::dynamic_pointer_cast<SessionUserIv>(pSession))->AddEvent(oEvent);
    return(true);
}

}
