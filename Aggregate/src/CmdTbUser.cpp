/*******************************************************************************
 * Project:  Nebio
 * @file     CmdTbUser.cpp
 * @brief    用户统计结果入口
 * @author   Bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/

#include <sstream>
#include "CmdTbUser.hpp"
#include "UnixTime.hpp"

namespace nebio
{

CmdTbUser::CmdTbUser(int32 iCmd)
   : neb::Cmd(iCmd),
     m_uiDate(0), m_dSessionTimeout(10.0)
{
}

CmdTbUser::~CmdTbUser()
{
}

bool CmdTbUser::Init()
{
    neb::CJsonObject oJsonConf = GetCustomConf();
    oJsonConf["analyse"]["session_timeout"].Get("session_tb_user", m_dSessionTimeout);
    m_uiDate = std::stoul(neb::time_t2TimeStr((time_t)GetNowTime(), "%Y%m%d"));
    m_strDate = neb::time_t2TimeStr((time_t)GetNowTime(), "%Y-%m-%d");
    return(true);
}

bool CmdTbUser::AnyMessage(
        std::shared_ptr<neb::SocketChannel> pChannel, 
        const MsgHead& oMsgHead, const MsgBody& oMsgBody)
{
    Result oResult;
    if (oResult.ParseFromString(oMsgBody.data()))
    {
        std::ostringstream oss;
        oss << "SessionTbUser-" << oResult.app_id() << "-" << oResult.channel() << "-" << oResult.tag() << "-" << oResult.key5();
        std::string strSessionId = oss.str();
        auto pSession = GetSession(strSessionId);
        if (pSession == nullptr)
        {
            pSession = MakeSharedSession("nebio::SessionTbUser", strSessionId, m_uiDate, m_strDate, m_dSessionTimeout);
        }
        if (pSession == nullptr)
        {
            return(false);
        }

        std::shared_ptr<SessionTbUser> pSessionTbUser = std::dynamic_pointer_cast<SessionTbUser>(pSession);
        pSessionTbUser->AddResult(oResult);
        return(true);
    }
    else
    {
        LOG4_ERROR("nebio::Result failed to parse MsgBody.data()!");
        return(false);
    }
}

}

