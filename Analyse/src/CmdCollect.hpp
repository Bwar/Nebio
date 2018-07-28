/*******************************************************************************
 * Project:  Nebio
 * @file     CmdCollect.hpp
 * @brief    事件入口
 * @author   Bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/

#ifndef NEBIO_CMDCOLLECT_HPP
#define NEBIO_CMDCOLLECT_HPP

#include <string>
#include <unordered_map>
#include <util/json/CJsonObject.hpp>
#include <actor/cmd/Cmd.hpp>
#include "event.pb.h"
#include "SessionSession.hpp"

namespace nebio
{

class CmdCollect : public neb::Cmd, public neb::DynamicCreator<CmdCollect, int32>
{
public:
    CmdCollect(int32 iCmd);
    virtual ~CmdCollect();

    virtual bool Init();

    virtual bool AnyMessage(
                    std::shared_ptr<neb::SocketChannel> pChannel, 
                    const MsgHead& oMsgHead,
                    const MsgBody& oMsgBody);
};

}  //namespace nebio 

#endif // NEBIO_CMDCOLLECT_HPP

