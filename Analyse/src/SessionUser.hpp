/*******************************************************************************
 * Project:  Nebio
 * @file     SessionUser.hpp
 * @brief    事件统计
 * @author   bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/
#ifndef SESSIONUSER_HPP_
#define SESSIONUSER_HPP_

#include <string>
#include <unordered_set>
#include <unordered_map>
#include "AnalyseCmd.hpp"
#include "AnalyseSession.hpp"

namespace nebio
{

enum E_USER_TYPE
{
    USER_UNDEFINE           = 0,
    USER_ACTIVITY           = 1,
    USER_NEW                = 2,
    USER_HISTORY            = 3,
    USER_TOURIST            = 4,
};

class SessionUser : public AnalyseSession,
    public neb::DynamicCreator<SessionUser, std::string, std::string, std::string, uint64, ev_tstamp>
{
public:
    SessionUser(const std::string& strSessionId, const std::string& strChannel, const std::string& strTag,
        uint64 ullStatDate, ev_tstamp dSessionTimeout = 10.0);
    virtual ~SessionUser();

    virtual neb::E_CMD_STATUS Timeout();

    void AddEvent(const Event& oEvent);

protected:
    void AddUser(const Event& oEvent);
    void AddTourist(const Event& oEvent);
    void DelTourist(const Event& oEvent);
    void SendResult();

private:
    uint64 m_ullStatDate;
    uint32 m_uiAppId;
    std::string m_strChannel;
    std::string m_strTag;

    uint32 m_uiActivityUserUv; 
    uint32 m_uiActivityUserPv;
    uint32 m_uiActivityUserVv;
    uint32 m_uiNewUserUv;
    uint32 m_uiNewUserPv;
    uint32 m_uiNewUserVv;
    uint32 m_uiHistoryUserUv;
    uint32 m_uiHistoryUserPv;
    uint32 m_uiHistoryUserVv;
    uint32 m_uiTouristUv;
    uint32 m_uiTouristPv;
    uint32 m_uiTouristVv;
    uint64 m_ullActivityUserSessionLength;
    uint64 m_ullNewUserSessionLength;
    uint64 m_ullHistoryUserSessionLength;
    uint64 m_ullTouristSessionLength;
    std::unordered_map<std::string, uint64> m_mapUser;
    std::unordered_set<std::string> m_setDayUser;
};

} // namespace nebio

#endif // SESSIONUSER_HPP_

