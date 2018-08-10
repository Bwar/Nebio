/*******************************************************************************
 * Project:  Nebio
 * @file     CmdTbPage.hpp
 * @brief    页面统计结果
 * @author   Bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/

#ifndef NEBIO_CMDTBPAGE_HPP
#define NEBIO_CMDTBPAGE_HPP

#include <string>
#include <unordered_map>
#include <util/json/CJsonObject.hpp>
#include <actor/cmd/Cmd.hpp>
#include "event.pb.h"
#include "SessionTbPage.hpp"

namespace nebio
{

class CmdTbPage : public neb::Cmd, public neb::DynamicCreator<CmdTbPage, int32>
{
public:
    CmdTbPage(int32 iCmd);
    virtual ~CmdTbPage();

    virtual bool Init();

    virtual bool AnyMessage(
                    std::shared_ptr<neb::SocketChannel> pChannel, 
                    const MsgHead& oMsgHead,
                    const MsgBody& oMsgBody);
private:
    double m_dSessionTimeout;
};

}

#endif // NEBIO_CMDTBPAGE_HPP

