/*******************************************************************************
 * Project:  Nebio
 * @file     CmdTbPage.cpp
 * @brief    页面统计结果
 * @author   Bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/

#include <sstream>
#include "CmdTbPage.hpp"
#include "UnixTime.hpp"

namespace nebio
{

CmdTbPage::CmdTbPage(int32 iCmd)
   : neb::Cmd(iCmd),
     m_uiDate(0), m_dSessionTimeout(10.0)
{
}

CmdTbPage::~CmdTbPage()
{
}

bool CmdTbPage::Init()
{
    neb::CJsonObject oJsonConf = GetCustomConf();
    oJsonConf["analyse"]["session_timeout"].Get("session_tb_page", m_dSessionTimeout);
    return(true);
}

bool CmdTbPage::AnyMessage(
        std::shared_ptr<neb::SocketChannel> pChannel, 
        const MsgHead& oMsgHead, const MsgBody& oMsgBody)
{
    Result oResult;
    if (oResult.ParseFromString(oMsgBody.data()))
    {
        std::ostringstream oss;
        oss << "SessionTbPage-" << oResult.app_id() << "-" << oResult.channel() << "-" << oResult.tag() << "-" << oResult.key1();
        std::string strSessionId = oss.str();
        auto pSession = GetSession(strSessionId);
        if (pSession == nullptr)
        {
            m_uiDate = std::stoul(neb::time_t2TimeStr((time_t)GetNowTime(), "%Y%m%d"));
            m_strDate = neb::time_t2TimeStr((time_t)GetNowTime(), "%Y-%m-%d");
            pSession = MakeSharedSession("nebio::SessionTbPage", strSessionId, m_uiDate, m_strDate, m_dSessionTimeout);
        }
        if (pSession == nullptr)
        {
            return(false);
        }

        std::shared_ptr<SessionTbPage> pSessionTbPage = std::dynamic_pointer_cast<SessionTbPage>(pSession);
        pSessionTbPage->AddResult(oResult);
        return(true);
    }
    else
    {
        LOG4_ERROR("nebio::Result failed to parse MsgBody.data()!");
        return(false);
    }
}

}

