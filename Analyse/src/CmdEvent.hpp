/*******************************************************************************
 * Project:  Nebio
 * @file     CmdEvent.hpp
 * @brief    事件入口
 * @author   Bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/

#ifndef NEBIO_CMDEVENT_HPP
#define NEBIO_CMDEVENT_HPP

#include <string>
#include <unordered_map>
#include <util/json/CJsonObject.hpp>
#include <actor/cmd/Cmd.hpp>
#include "event.pb.h"
#include "SessionEvent.hpp"

namespace nebio
{

class CmdEvent : public neb::Cmd, public neb::DynamicCreator<CmdEvent, int32>
{
public:
    CmdEvent(int32 iCmd);
    virtual ~CmdEvent();

    virtual bool Init();

    virtual bool AnyMessage(
                    std::shared_ptr<neb::SocketChannel> pChannel, 
                    const MsgHead& oMsgHead,
                    const MsgBody& oMsgBody);

protected:
    bool Stat(const std::string& strChannel, const std::string& strTag, const Event& oEvent);

private:
    std::string m_strChannelSummary;
    std::string m_strTagSummary;
};

}

#endif // NEBIO_CMD_EVENT_HPP

