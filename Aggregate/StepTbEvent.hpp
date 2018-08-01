/*******************************************************************************
 * Project:  AggregateServer
 * @file     StepTbEvent.hpp
 * @brief    将http请求转换成内部协议到Logic服务器进行处理
 * @author   lbh
 * @date:    2016年7月6日
 * @note
 * Modify history:
 ******************************************************************************/
#ifndef NEBIO_STEPTBEVENT_HPP
#define NEBIO_STEPTBEVENT_HPP

#include <actor/step/PbStep.hpp>

namespace nebio 
{

class StepTbEvent: public neb::PbStep, public neb::DynamicCreator<StepTbEvent>
{
public:
    StepTbEvent();
    virtual ~StepTbEvent();

    virtual neb::E_CMD_STATUS Emit(int iErrno, const std::string& strErrMsg = "",  void* data = NULL);

    virtual neb::E_CMD_STATUS Callback(
                    std::shared_ptr<neb::SocketChannel> pUpstreamChannel,
                    const MsgHead& oInMsgHead,
                    const MsgBody& oInMsgBody,
                    void* data = NULL);

    virtual neb::E_CMD_STATUS Timeout();
};

} /* namespace nebio*/

#endif /* NEBIO_STEPTBEVENT_HPP_ */
