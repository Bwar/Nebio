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

namespace nebio
{

CmdTbUser::CmdTbUser(int32 iCmd)
   : neb::Cmd(iCmd)
{
}

CmdTbUser::~CmdTbUser()
{
}

bool CmdTbUser::Init()
{
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
        oss << "SessionTbUser-" + oResult.app_id() << "-" << oResult.channel() << "-" << oResult.tag() << "-" << oResult.key5();
        std::string strSessionId = oss.str();
        auto pSession = GetSession(strSessionId);
        if (pSession == nullptr)
        {
            pSession = MakeSharedSession("nebio::SessionTbUser", strSessionId, 10.0);
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

