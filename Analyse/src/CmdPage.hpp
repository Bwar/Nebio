/*******************************************************************************
 * Project:  Nebio
 * @file     CmdPage.hpp
 * @brief    页面入口
 * @author   Bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/

#ifndef NEBIO_CMDPAGE_HPP
#define NEBIO_CMDPAGE_HPP

#include <string>
#include <unordered_map>
#include <util/json/CJsonObject.hpp>
#include <actor/cmd/Cmd.hpp>
#include "event.pb.h"
#include "SessionPage.hpp"

namespace nebio
{

class CmdPage : public neb::Cmd, public neb::DynamicCreator<CmdPage, int32>
{
public:
    CmdPage(int32 iCmd);
    virtual ~CmdPage();

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

#endif // NEBIO_CMD_EVENT_HPP

