/*******************************************************************************
 * Project:  Nebio
 * @file     CmdTbUser.hpp
 * @brief    用户统计结果入口
 * @author   Bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/

#ifndef NEBIO_CMDTBUSER_HPP
#define NEBIO_CMDTBUSER_HPP

#include <string>
#include <unordered_map>
#include <util/json/CJsonObject.hpp>
#include <actor/cmd/Cmd.hpp>
#include "event.pb.h"
#include "SessionTbUser.hpp"

namespace nebio
{

class CmdTbUser : public neb::Cmd, public neb::DynamicCreator<CmdTbUser, int32>
{
public:
    CmdTbUser(int32 iCmd);
    virtual ~CmdTbUser();

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

