/*******************************************************************************
 * Project:  Nebio
 * @file     AnalyseSession.hpp
 * @brief    会话统计
 * @author   Bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/
#ifndef NEBIO_ANALYSESESSION_HPP
#define NEBIO_ANALYSESESSION_HPP

#include <actor/session/Session.hpp>
#include "event.pb.h"
#include "AnalyseCmd.hpp"

namespace nebio
{

class AnalyseSession : public neb::Session
{
public:
    AnalyseSession(const std::string& strSessionId, ev_tstamp dSessionTimeout)
        : neb::Session(strSessionId, dSessionTimeout)
    {        
    }

    virtual ~AnalyseSession(){}

    virtual neb::E_CMD_STATUS Timeout() = 0;

    virtual void AddEvent(const Event& oEvent) = 0;
};

}

#endif

