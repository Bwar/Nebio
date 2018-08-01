/*******************************************************************************
 * Project:  AggregateServer
 * @file     StepTbEvent.cpp
 * @brief 
 * @author   Bwar
 * @date:    2018年7月8日
 * @note
 * Modify history:
 ******************************************************************************/
#include <mydis/DbOperator.hpp>
#include "StepTbEvent.hpp"

namespace nebio
{

StepTbEvent::StepTbEvent()
{
}

StepTbEvent::~StepTbEvent()
{
}

neb::E_CMD_STATUS StepTbEvent::Emit(int iErrno, const std::string& strErrMsg, void* data)
{
    LOG4_DEBUG("%s()", __FUNCTION__);
    MsgBody oMsgBody;
    oMsgBody.set_data(((neb::Mydis*)data)->SerializeAsString());
    SendOriented("DBAGENT", neb::CMD_REQ_STORATE, GetSequence(), oMsgBody);
    return(neb::CMD_STATUS_RUNNING);
}

neb::E_CMD_STATUS StepTbEvent::Callback(
        std::shared_ptr<neb::SocketChannel> pUpstreamChannel,
        const MsgHead& oInMsgHead,
        const MsgBody& oInMsgBody,
        void* data)
{
    LOG4_TRACE("%s()", __FUNCTION__);
    return(neb::CMD_STATUS_COMPLETED);
}

neb::E_CMD_STATUS StepTbEvent::Timeout()
{
    LOG4_DEBUG("%s()", __FUNCTION__);
    return(neb::CMD_STATUS_FAULT);
}

} /* namespace nebio */
