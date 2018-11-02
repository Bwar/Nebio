/*******************************************************************************
 * Project:  Nebio
 * @file     CmdUser.hpp
 * @brief    用户入口
 * @author   Bwar
 * @date:    2018年4月22日
 * @note     
 * Modify history:
 ******************************************************************************/

#ifndef NEBIO_CMDUSER_HPP
#define NEBIO_CMDUSER_HPP

#include <string>
#include <unordered_map>
#include <util/json/CJsonObject.hpp>
#include <actor/cmd/Cmd.hpp>
#include "event.pb.h"
#include "SessionUser.hpp"

namespace nebio
{

class CmdUser : public neb::Cmd, public neb::DynamicCreator<CmdUser, int32>
{
public:
    CmdUser(int32 iCmd);
    virtual ~CmdUser();

    virtual bool Init();

    virtual bool AnyMessage(
                    std::shared_ptr<neb::SocketChannel> pChannel, 
                    const MsgHead& oMsgHead,
                    const MsgBody& oMsgBody);

protected:
    bool Stat(const std::string& strChannel, const std::string& strTag, const Event& oEvent, bool bAlwaysOnline = false);

private:
    uint32 m_uiDate;        // YYYYMMDD
    double m_dSessionTimeout;
    std::string m_strChannelSummary;
    std::string m_strTagSummary;
};

}

#endif // NEBIO_CMDUSER_HPP

