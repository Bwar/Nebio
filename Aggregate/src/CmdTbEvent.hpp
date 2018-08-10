/*******************************************************************************
 * Project:  Nebio
 * @file     CmdTbEvent.hpp
 * @brief    事件入口
 * @author   Bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/

#ifndef NEBIO_CMDTBEVENT_HPP
#define NEBIO_CMDTBEVENT_HPP

#include <string>
#include <unordered_map>
#include <util/json/CJsonObject.hpp>
#include <actor/cmd/Cmd.hpp>
#include "event.pb.h"
#include "SessionTbEvent.hpp"

namespace nebio
{

class CmdTbEvent : public neb::Cmd, public neb::DynamicCreator<CmdTbEvent, int32>
{
public:
    CmdTbEvent(int32 iCmd);
    virtual ~CmdTbEvent();

    virtual bool Init();

    virtual bool AnyMessage(
                    std::shared_ptr<neb::SocketChannel> pChannel, 
                    const MsgHead& oMsgHead,
                    const MsgBody& oMsgBody);
private:
    double m_dSessionTimeout;
};

}

#endif // NEBIO_CMD_EVENT_HPP

