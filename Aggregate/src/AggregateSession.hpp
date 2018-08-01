/*******************************************************************************
 * Project:  Nebio
 * @file     AggregateSession.hpp
 * @brief    会话统计
 * @author   Bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/
#include <actor/session/Session.hpp>
#include "event.pb.h"

namespace nebio
{

class AggregateSession : public neb::Session
{
public:
    AggregateSession(const std::string& strSessionId, ev_tstamp dSessionTimeout)
        : neb::Session(strSessionId, dSessionTimeout)
    {        
    }

    virtual ~AggregateSession(){}

    virtual neb::E_CMD_STATUS Timeout() = 0;

    virtual void AddResult(const Result& oResult) = 0;
};

}
