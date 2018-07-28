/*******************************************************************************
 * Project:  Nebio
 * @file     AnalyseSession.hpp
 * @brief    会话统计
 * @author   Bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/
#include <actor/session/Session.hpp>
#include "event.pb.h"
#include "AnalyseCmd.hpp"

namespace nebio
{

class AnalyseSession : public neb::Session
{
public:
    AnalyseSession(const std::string& strSessionId, ev_tstamp dSessionTimeout, const std::string& strSessionClass)
        : neb::Session(strSessionId, dSessionTimeout, strSessionClass)
    {        
    }

    virtual ~AnalyseSession(){}

    virtual neb::E_CMD_STATUS Timeout() = 0;

    virtual void AddEvent(const Event& oEvent) = 0;
};

}
