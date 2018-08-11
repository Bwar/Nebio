/*******************************************************************************
 * Project:  Nebio
 * @file     SessionSession.hpp
 * @brief    会话统计
 * @author   Bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/
#ifndef SESSIONSESSION_HPP_
#define SESSIONSESSION_HPP_

#include <string>
#include <list>
#include <tuple>
#include <unordered_set>
#include "AnalyseSession.hpp"

namespace nebio
{

// event_id, event_type, time, page 
typedef std::tuple<std::string, std::string, uint64, std::string> T_EVENT; 

// page, time
typedef std::tuple<std::string, uint64> T_PAGE;

class SessionSession: public AnalyseSession,
    public neb::DynamicCreator<SessionSession, std::string, uint32, ev_tstamp>
{
public:
    SessionSession(const std::string& strSessionId, uint32 uiDate, ev_tstamp dSessionTimeout = 1800.0);
    virtual ~SessionSession();

    virtual neb::E_CMD_STATUS Timeout();

    void AddEvent(const Event& oEvent);

protected:
    /**
     * @brief 事件转发
     * @param iEventPos 转发第几个事件： -1 m_listEvent的末尾事件， -2 m_listEvent从末尾数过来的第二个事件
     */
    void TransferEvent(int iEventPos = -2);

    /**
     * @brief 页面事件转发
     * @param iEventPos 转发第几个事件： -1 m_listEvent的末尾事件， -2 m_listEvent从末尾数过来的第二个事件
     */
    void TransferPageEvent(int iEventPos = -2);

    void TransferUserEvent(int iEventPos = -2);

private:
    uint32 m_uiDate;        // YYYYMMDD
    bool m_bTourist2User;                // 是否从游客转为注册用户
    uint32 m_uiAppId;
    std::string m_strUserSessionId;      // GetSessionId()获得的是 appid-session_id的组合
    std::string m_strUserId;
    std::string m_strTouristId;
    std::string m_strDeviceId;
    std::string m_strPlat;
    std::string m_strReferer;
    std::string m_strExplorer;
    std::string m_strClientIp;
    std::list<T_EVENT> m_listEvent;
    std::list<T_PAGE> m_listPage;
    std::unordered_set<std::string> m_setEventVv;
    std::unordered_set<std::string> m_setPageVv;
};

}   // namespace nebio

#endif  // SESSIONSESSION_HPP_

