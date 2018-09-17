/*******************************************************************************
 * Project:  Nebio
 * @file     CmdCollect.cpp
 * @brief    事件入口
 * @author   Bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/

#include "util/StringCoder.hpp"
#include "CmdCollect.hpp"
#include "UnixTime.hpp"

namespace nebio
{

CmdCollect::CmdCollect(int32 iCmd)
   : neb::Cmd(iCmd), m_uiDate(0), m_dSessionTimeout(1200.0)
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
        std::string strUrl = "";
        std::string strParam = "";
        std::map<std::string, std::string> mapParam;
        strParam = std::move(oEvent.page().substr(oEvent.page().find_first_of('?') + 1, std::string::npos));
        if (strParam.size() > 0)
        {
            neb::DecodeParameter(strParam, mapParam);
        }
        for (auto it = mapParam.begin(); it != mapParam.end(); ++it)
        {
            if (it->first == "ADTAG")
            {
                oEvent.set_tag(it->second);
                break;
            }
        }
        strUrl = std::move(oEvent.page().substr(0, oEvent.page().find_first_of('?')));
        strUrl = std::move(strUrl.substr(0, strUrl.find_first_of('#')));
        strUrl = std::move(strUrl.substr(0, strUrl.find_last_not_of('/')+ 1));
        oEvent.set_page(strUrl);
        strUrl = std::move(oEvent.referer().substr(0, oEvent.referer().find_first_of('?')));
        strUrl = std::move(strUrl.substr(0, strUrl.find_first_of('#')));
        strUrl = std::move(strUrl.substr(0, strUrl.find_last_not_of('/')+ 1));
        if (strUrl == oEvent.page())
        {
            oEvent.set_referer("");
        }
        else
        {
            oEvent.set_referer(strUrl);
        }

        std::string strSessionId = std::string("SessionSession-") + std::to_string(oEvent.app_id()) + "-" + oEvent.session_id();
        auto pSession = GetSession(strSessionId);
        if (pSession == nullptr)
        {
            m_uiDate = std::stoul(neb::time_t2TimeStr((time_t)GetNowTime(), "%Y%m%d"));
            pSession = MakeSharedSession("nebio::SessionSession", strSessionId, m_uiDate, m_dSessionTimeout);
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

