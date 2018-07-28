/*******************************************************************************
 * Project:  Nebio
 * @file     SessionTbSession.hpp
 * @brief    会话统计
 * @author   bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/
#ifndef SESSIONTBSESSION_HPP_
#define SESSIONTBSESSION_HPP_

#include <string>
#include <unordered_set>
#include "actor/session/Session.hpp"

namespace nebio
{

class SessionTbSession: public neb::Session,
    public DynamicCreator<SessionTbSession, std::string, ev_tstamp, std::string>
{
public:
    typedef std::tuple<uint32, uint32, uint32> T_TB_SESSION_VALUE;

    SessionTbSession(const std::string& strSessionId,
            ev_tstamp dSessionTimeout = 10.0,
            const std::string& strSessionClass = "nebio::SessionEvent");
    virtual ~SessionTbSession();

private:
    uint32 m_uiAppId;
    std::string m_strDate;
    std::string m_strChannel;
};

}    // namespace nebio

#endif // SESSIONTBSESSION_HPP__

