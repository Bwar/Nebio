/*******************************************************************************
 * Project:  Nebio
 * @file     CmdPageIv.hpp
 * @brief    页面入口
 * @author   Bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/

#ifndef NEBIO_CMDPAGEIV_HPP
#define NEBIO_CMDPAGEIV_HPP

#include <string>
#include <unordered_map>
#include <util/json/CJsonObject.hpp>
#include <actor/cmd/Cmd.hpp>
#include "event.pb.h"
#include "SessionPageIv.hpp"

namespace nebio
{

class CmdPageIv : public neb::Cmd, public neb::DynamicCreator<CmdPageIv, int32>
{
public:
    CmdPageIv(int32 iCmd);
    virtual ~CmdPageIv();

    virtual bool Init();

    virtual bool AnyMessage(
                    std::shared_ptr<neb::SocketChannel> pChannel, 
                    const MsgHead& oMsgHead,
                    const MsgBody& oMsgBody);

protected:
    bool Stat(const std::string& strChannel, const std::string& strTag, const Event& oEvent);

private:
    double m_dSessionTimeout;
    std::string m_strChannelSummary;
    std::string m_strTagSummary;
};

}

#endif // NEBIO_CMD_EVENTIV_HPP

