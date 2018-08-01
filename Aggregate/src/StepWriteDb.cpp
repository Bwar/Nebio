/*******************************************************************************
 * Project:  AggregateServer
 * @file     StepWriteDb.cpp
 * @brief 
 * @author   Bwar
 * @date:    2018年7月8日
 * @note
 * Modify history:
 ******************************************************************************/
#include <mydis/DbOperator.hpp>
#include "StepWriteDb.hpp"

namespace nebio
{

StepWriteDb::StepWriteDb()
{
}

StepWriteDb::~StepWriteDb()
{
}

neb::E_CMD_STATUS StepWriteDb::Emit(int iErrno, const std::string& strErrMsg, void* data)
{
    LOG4_DEBUG("%s()", __FUNCTION__);
    MsgBody oMsgBody;
    oMsgBody.set_data(((neb::Mydis*)data)->SerializeAsString());
    SendOriented("DBAGENT", neb::CMD_REQ_STORATE, GetSequence(), oMsgBody);
    return(neb::CMD_STATUS_RUNNING);
}

neb::E_CMD_STATUS StepWriteDb::Callback(
        std::shared_ptr<neb::SocketChannel> pUpstreamChannel,
        const MsgHead& oInMsgHead,
        const MsgBody& oInMsgBody,
        void* data)
{
    LOG4_TRACE("%s()", __FUNCTION__);
    return(neb::CMD_STATUS_COMPLETED);
}

neb::E_CMD_STATUS StepWriteDb::Timeout()
{
    LOG4_DEBUG("%s()", __FUNCTION__);
    return(neb::CMD_STATUS_FAULT);
}

} /* namespace nebio */
