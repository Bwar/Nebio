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
#include "AnalyseTimer.hpp"

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

class SessionUser : public AnalyseTimer,
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
    void FlushOut();

private:
    uint64 m_ullStatDate;
    uint32 m_uiAppId;
    std::string m_strChannel;
    std::string m_strTag;

    int32 m_iActivityUserUv; 
    int32 m_iActivityUserPv;
    int32 m_iActivityUserVv;
    int32 m_iNewUserUv;
    int32 m_iNewUserPv;
    int32 m_iNewUserVv;
    int32 m_iHistoryUserUv;
    int32 m_iHistoryUserPv;
    int32 m_iHistoryUserVv;
    int32 m_iTouristUv;
    int32 m_iTouristPv;
    int32 m_iTouristVv;
    int64 m_llActivityUserSessionLength;
    int64 m_llNewUserSessionLength;
    int64 m_llHistoryUserSessionLength;
    int64 m_llTouristSessionLength;
    std::unordered_map<std::string, uint64> m_mapUser;
    std::unordered_set<std::string> m_setDayUser;
};

} // namespace nebio

#endif // SESSIONUSER_HPP_

