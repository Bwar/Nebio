/*******************************************************************************
 * Project:  Nebio
 * @file     CmdPage.cpp
 * @brief    页面入口
 * @author   Bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/

#include <sstream>
#include "CmdPage.hpp"

namespace nebio
{

CmdPage::CmdPage(int32 iCmd)
   : neb::Cmd(iCmd)
{
}

CmdPage::~CmdPage()
{
}

bool CmdPage::Init()
{
    neb::CJsonObject oJsonConf = GetCustomConf();
    m_strChannelSummary = oJsonConf["analyse"]("channel_summary");
    m_strTagSummary = oJsonConf["analyse"]("tag_summary");
    return(true);
}

bool CmdPage::AnyMessage(
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

bool CmdPage::Stat(const std::string& strChannel, const std::string& strTag, const Event& oEvent)
{
    if (strChannel.length() == 0 || strTag.length() == 0)
    {
        return(true);
    }
    
    std::ostringstream oss;
    oss << "SessionPage-" + oEvent.app_id() << "-" << strChannel << "-" << strTag << "-" << oEvent.event_id();
    std::string strSessionId = oss.str();
    auto pSession = GetSession(strSessionId);
    if (pSession == nullptr)
    {
        pSession = MakeSharedSession("nebio::SessionPage", strSessionId, strChannel, strTag, 10.0);
    }
    if (pSession == nullptr)
    {
        return(false);
    }

    (std::dynamic_pointer_cast<SessionPage>(pSession))->AddEvent(oEvent);
    return(true);
}

}
