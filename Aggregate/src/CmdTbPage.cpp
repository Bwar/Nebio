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

namespace nebio
{

CmdTbPage::CmdTbPage(int32 iCmd)
   : neb::Cmd(iCmd)
{
}

CmdTbPage::~CmdTbPage()
{
}

bool CmdTbPage::Init()
{
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
        oss << "SessionTbPage-" + oResult.app_id() << "-" << oResult.channel() << "-" << oResult.tag() << "-" << oResult.key1();
        std::string strSessionId = oss.str();
        auto pSession = GetSession(strSessionId);
        if (pSession == nullptr)
        {
            pSession = MakeSharedSession("nebio::SessionTbPage", strSessionId, 10.0);
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

