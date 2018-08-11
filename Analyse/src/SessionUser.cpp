/*******************************************************************************
 * Project:  Nebio
 * @file     SessionUser.cpp
 * @brief    事件统计
 * @author   bwar
 * @date:    2018年4月22日
 * @note     
 * Modify history:
 ******************************************************************************/

#include "SessionUser.hpp"
#include "UnixTime.hpp"

namespace nebio
{

SessionUser::SessionUser(const std::string& strSessionId,
    const std::string& strChannel, const std::string& strTag, uint32 uiDate, ev_tstamp dSessionTimeout)
    : AnalyseTimer(strSessionId, dSessionTimeout),
      m_uiDate(uiDate), m_ullStatDate(0), m_uiAppId(0), m_strChannel(strChannel), m_strTag(strTag),
      m_iActivityUserUv(0), m_iActivityUserPv(0), m_iActivityUserVv(0), 
      m_iNewUserUv(0), m_iNewUserPv(0), m_iNewUserVv(0), 
      m_iHistoryUserUv(0), m_iHistoryUserPv(0), m_iHistoryUserVv(0), 
      m_iTouristUv(0), m_iTouristPv(0), m_iTouristVv(0), 
      m_llActivityUserSessionLength(0), m_llNewUserSessionLength(0),
      m_llHistoryUserSessionLength(0), m_llTouristSessionLength(0)
{
    m_ullStatDate =  neb::GetBeginTimeOfTheDay(time(NULL));
}

SessionUser::~SessionUser()
{
    m_mapUser.clear();
    m_setDayUser.clear();
}

neb::E_CMD_STATUS SessionUser::Timeout()
{
    FlushOut();
    uint32 uiDate = std::stoul(neb::time_t2TimeStr((time_t)GetNowTime(), "%Y%m%d"));
    if (uiDate > m_uiDate)
    {
        m_uiDate = uiDate;
        m_ullStatDate = neb::GetBeginTimeOfTheDay((time_t)GetNowTime());
        m_setDayUser.clear();
    }
    return(neb::CMD_STATUS_RUNNING);
}

void SessionUser::AddEvent(const Event& oEvent)
{
    LOG4_TRACE("%s", oEvent.DebugString().c_str());
    if (m_uiAppId == 0)
    {
        m_uiAppId = oEvent.app_id();
    }

    if (Event::EVENT_ADD == oEvent.event_oper())
    {
        if (oEvent.user_id().length() > 0)
        {
            AddUser(oEvent);
        }
        else
        {
            AddTourist(oEvent);
        }
    }
    else
    {
        DelTourist(oEvent);
    }
}

void SessionUser::AddUser(const Event& oEvent)
{
    LOG4_TRACE("%s", oEvent.DebugString().c_str());
    bool bIsNewUser = false;
    auto user_iter = m_mapUser.find(oEvent.user_id());
    if (user_iter == m_mapUser.end())
    {
        bIsNewUser = true;
        m_mapUser.insert(std::make_pair(oEvent.user_id(), oEvent.time()));
    }
    else
    {
        if (user_iter->second >= m_ullStatDate)
        {
            bIsNewUser = true;
        }
    }

    ++m_iActivityUserPv;
    m_iActivityUserVv += oEvent.vv();
    m_llActivityUserSessionLength += oEvent.length();
    auto day_user_iter = m_setDayUser.find(oEvent.user_id());
    if (day_user_iter == m_setDayUser.end())
    {
        m_setDayUser.insert(oEvent.user_id());
        ++m_iActivityUserUv;
    }

    MsgBody oMsgBody;
    Event oIpEvent = oEvent;
    if (bIsNewUser)
    {
        ++m_iNewUserPv;
        m_iNewUserVv += oEvent.vv();
        m_llNewUserSessionLength += oEvent.length();
        if (day_user_iter == m_setDayUser.end())
        {
            ++m_iNewUserUv;
        }
        oIpEvent.set_user_type(USER_NEW);
    }
    else
    {
        ++m_iHistoryUserPv;
        m_iHistoryUserVv += oEvent.vv();
        m_llHistoryUserSessionLength += oEvent.length();
        if (day_user_iter == m_setDayUser.end())
        {
            ++m_iHistoryUserUv;
        }
        oIpEvent.set_user_type(USER_HISTORY);
    }
    oMsgBody.set_data(oIpEvent.SerializeAsString());
    oMsgBody.mutable_req_target()->set_route(oIpEvent.client_ip());
    SendOriented("ANALYSE", CMD_USER_IV, GetSequence(), oMsgBody);
}

void SessionUser::AddTourist(const Event& oEvent)
{
    LOG4_TRACE("%s", oEvent.DebugString().c_str());
    ++m_iActivityUserPv;
    m_iActivityUserVv += oEvent.vv();
    m_llActivityUserSessionLength += oEvent.length();
    ++m_iTouristPv;
    m_iTouristVv += oEvent.vv();
    m_llTouristSessionLength += oEvent.length();
    auto day_user_iter = m_setDayUser.find(oEvent.tourist_id());
    if (day_user_iter == m_setDayUser.end())
    {
        m_setDayUser.insert(oEvent.tourist_id());
        ++m_iActivityUserUv;
        ++m_iTouristUv;
    }
    MsgBody oMsgBody;
    Event oIpEvent = oEvent;
    oIpEvent.set_user_type(USER_TOURIST);
    oMsgBody.set_data(oIpEvent.SerializeAsString());
    oMsgBody.mutable_req_target()->set_route(oIpEvent.client_ip());
    SendOriented("ANALYSE", CMD_USER_IV, GetSequence(), oMsgBody);
}

void SessionUser::DelTourist(const Event& oEvent)
{
    LOG4_TRACE("%s", oEvent.DebugString().c_str());
    --m_iActivityUserPv;
    m_iActivityUserVv -= oEvent.vv();
    m_llActivityUserSessionLength -= oEvent.length();
    --m_iTouristPv;
    m_iTouristVv -= oEvent.vv();
    m_llTouristSessionLength -= oEvent.length();
    auto day_user_iter = m_setDayUser.find(oEvent.tourist_id());
    if (day_user_iter != m_setDayUser.end())
    {
        m_setDayUser.erase(day_user_iter);
        --m_iActivityUserUv;
        --m_iTouristUv;
    }
}

void SessionUser::FlushOut()
{
    MsgBody oMsgBody;
    Result oResult;
    oMsgBody.mutable_req_target()->set_route_id(m_uiAppId);

    oResult.set_app_id(m_uiAppId);
    oResult.set_channel(m_strChannel);
    oResult.set_tag(m_strTag);

    oResult.set_key5(USER_ACTIVITY);
    oResult.set_pv(m_iActivityUserPv);
    oResult.set_uv(m_iActivityUserUv);
    oResult.set_vv(m_iActivityUserVv);
    oResult.set_length(m_llActivityUserSessionLength);
    oMsgBody.set_data(oResult.SerializeAsString());
    oMsgBody.mutable_req_target()->set_route_id(m_uiAppId);
    SendOriented("AGGREGATE", CMD_TB_USER, GetSequence(), oMsgBody);

    oResult.set_key5(USER_NEW);
    oResult.set_pv(m_iNewUserPv);
    oResult.set_uv(m_iNewUserUv);
    oResult.set_vv(m_iNewUserVv);
    oResult.set_length(m_llNewUserSessionLength);
    oMsgBody.set_data(oResult.SerializeAsString());
    oMsgBody.mutable_req_target()->set_route_id(m_uiAppId);
    SendOriented("AGGREGATE", CMD_TB_USER, GetSequence(), oMsgBody);

    oResult.set_key5(USER_HISTORY);
    oResult.set_pv(m_iHistoryUserPv);
    oResult.set_uv(m_iHistoryUserUv);
    oResult.set_vv(m_iHistoryUserVv);
    oResult.set_length(m_llHistoryUserSessionLength);
    oMsgBody.set_data(oResult.SerializeAsString());
    oMsgBody.mutable_req_target()->set_route_id(m_uiAppId);
    SendOriented("AGGREGATE", CMD_TB_USER, GetSequence(), oMsgBody);

    oResult.set_key5(USER_TOURIST);
    oResult.set_pv(m_iTouristPv);
    oResult.set_uv(m_iTouristUv);
    oResult.set_vv(m_iTouristVv);
    oResult.set_length(m_llTouristSessionLength);
    oMsgBody.set_data(oResult.SerializeAsString());
    oMsgBody.mutable_req_target()->set_route_id(m_uiAppId);
    SendOriented("AGGREGATE", CMD_TB_USER, GetSequence(), oMsgBody);

    m_iActivityUserPv = 0;
    m_iActivityUserUv = 0;
    m_iActivityUserVv = 0;
    m_iHistoryUserPv = 0;
    m_iHistoryUserUv = 0;
    m_iHistoryUserVv = 0;
    m_iNewUserPv = 0;
    m_iNewUserUv = 0;
    m_iNewUserVv = 0;
    m_iTouristPv = 0;
    m_iTouristUv = 0;
    m_iTouristVv = 0;
    m_llActivityUserSessionLength = 0;
    m_llHistoryUserSessionLength = 0;
    m_llNewUserSessionLength = 0;
    m_llTouristSessionLength = 0;
}

}
