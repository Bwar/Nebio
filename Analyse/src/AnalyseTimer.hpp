/*******************************************************************************
 * Project:  Nebio
 * @file     AnalyseTimer.hpp
 * @brief    会话统计
 * @author   Bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/
#ifndef NEBIO_ANALYSETIMER_HPP
#define NEBIO_ANALYSETIMER_HPP

#include <actor/session/Timer.hpp>
#include "event.pb.h"
#include "AnalyseCmd.hpp"

namespace nebio
{

class AnalyseTimer : public neb::Timer
{
public:
    AnalyseTimer(const std::string& strSessionId, ev_tstamp dSessionTimeout)
        : neb::Timer(strSessionId, dSessionTimeout)
    {        
    }

    virtual ~AnalyseTimer(){}

    virtual neb::E_CMD_STATUS Timeout() = 0;

    virtual void AddEvent(const Event& oEvent) = 0;
};

}

#endif

