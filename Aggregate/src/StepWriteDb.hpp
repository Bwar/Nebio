/*******************************************************************************
 * Project:  AggregateServer
 * @file     StepWriteDb.hpp
 * @brief    
 * @author   Bwar
 * @date:    2018年7月8日
 * @note
 * Modify history:
 ******************************************************************************/
#ifndef NEBIO_STEPTBPAGE_HPP
#define NEBIO_STEPTBPAGE_HPP

#include <actor/step/PbStep.hpp>

namespace nebio 
{

class StepWriteDb: public neb::PbStep, public neb::DynamicCreator<StepWriteDb>
{
public:
    StepWriteDb();
    virtual ~StepWriteDb();

    virtual neb::E_CMD_STATUS Emit(int iErrno, const std::string& strErrMsg = "",  void* data = NULL);

    virtual neb::E_CMD_STATUS Callback(
                    std::shared_ptr<neb::SocketChannel> pUpstreamChannel,
                    const MsgHead& oInMsgHead,
                    const MsgBody& oInMsgBody,
                    void* data = NULL);

    virtual neb::E_CMD_STATUS Timeout();
};

} /* namespace nebio*/

#endif /* NEBIO_STEPTBPAGE_HPP_ */
