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
   : neb::Cmd(iCmd)
{
}

CmdCollect::~CmdCollect()
{
}

bool CmdCollect::Init()
{
    return(true);
}

bool CmdCollect::AnyMessage(
        std::shared_ptr<neb::SocketChannel> pChannel, 
        const MsgHead& oMsgHead, const MsgBody& oMsgBody)
{
    Event oEvent;
    if (oEvent.ParseFromString(oMsgBody.data()))
    {
        std::string strSessionId = std::to_string(oEvent.app_id()) + oEvent.event_id();
        auto pSession = GetSession(strSessionId, "nebio::SessionSession");
        if (pSession == nullptr)
        {
            pSession = MakeSharedSession("nebio::SessionSession", strSessionId);
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

