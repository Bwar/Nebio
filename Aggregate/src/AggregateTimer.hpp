/*******************************************************************************
 * Project:  Nebio
 * @file     AggregateTimer.hpp
 * @brief    会话统计
 * @author   Bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/
#ifndef NEBIO_AGGREGATETIMER_HPP
#define NEBIO_AGGREGATETIMER_HPP

#include <actor/session/Timer.hpp>
#include "event.pb.h"

namespace nebio
{

class AggregateTimer : public neb::Timer
{
public:
    AggregateTimer(const std::string& strSessionId, ev_tstamp dSessionTimeout)
        : neb::Timer(strSessionId, dSessionTimeout)
    {        
    }

    virtual ~AggregateTimer(){}

    virtual neb::E_CMD_STATUS Timeout() = 0;

    virtual void AddResult(const Result& oResult) = 0;
};

}

#endif


