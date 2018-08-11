/*******************************************************************************
 * Project:  Nebio
 * @file     CmdEventIv.hpp
 * @brief    事件入口
 * @author   Bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/

#ifndef NEBIO_CMDEVENTIV_HPP
#define NEBIO_CMDEVENTIV_HPP

#include <string>
#include <unordered_map>
#include <util/json/CJsonObject.hpp>
#include <actor/cmd/Cmd.hpp>
#include "event.pb.h"
#include "SessionEventIv.hpp"

namespace nebio
{

class CmdEventIv : public neb::Cmd, public neb::DynamicCreator<CmdEventIv, int32>
{
public:
    CmdEventIv(int32 iCmd);
    virtual ~CmdEventIv();

    virtual bool Init();

    virtual bool AnyMessage(
                    std::shared_ptr<neb::SocketChannel> pChannel, 
                    const MsgHead& oMsgHead,
                    const MsgBody& oMsgBody);

protected:
    bool Stat(const std::string& strChannel, const std::string& strTag, const Event& oEvent);

private:
    uint32 m_uiDate;        // YYYYMMDD
    double m_dSessionTimeout;
    std::string m_strChannelSummary;
    std::string m_strTagSummary;
};

}

#endif // NEBIO_CMD_EVENTIV_HPP

