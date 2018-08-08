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

namespace nebio
{

SessionUser::SessionUser(const std::string& strSessionId,
    const std::string& strChannel, const std::string& strTag, uint64 ullStatDate, ev_tstamp dSessionTimeout)
    : AnalyseSession(strSessionId, dSessionTimeout),
      m_ullStatDate(ullStatDate), m_uiAppId(0), m_strChannel(strChannel), m_strTag(strTag),
      m_uiActivityUserUv(0), m_uiActivityUserPv(0), m_uiActivityUserVv(0), 
      m_uiNewUserUv(0), m_uiNewUserPv(0), m_uiNewUserVv(0), 
      m_uiHistoryUserUv(0), m_uiHistoryUserPv(0), m_uiHistoryUserVv(0), 
      m_uiTouristUv(0), m_uiTouristPv(0), m_uiTouristVv(0), 
      m_ullActivityUserSessionLength(0), m_ullNewUserSessionLength(0),
      m_ullHistoryUserSessionLength(0), m_ullTouristSessionLength(0)
{
}

SessionUser::~SessionUser()
{
    m_mapUser.clear();
}

neb::E_CMD_STATUS SessionUser::Timeout()
{
    SendResult();
    return(neb::CMD_STATUS_RUNNING);
}

void SessionUser::AddEvent(const Event& oEvent)
{
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

    ++m_uiActivityUserPv;
    m_uiActivityUserVv += oEvent.vv();
    m_ullActivityUserSessionLength += oEvent.length();
    auto day_user_iter = m_setDayUser.find(oEvent.user_id());
    if (day_user_iter == m_setDayUser.end())
    {
        m_setDayUser.insert(oEvent.user_id());
        ++m_uiActivityUserUv;
    }

    MsgBody oMsgBody;
    Event oIpEvent = oEvent;
    if (bIsNewUser)
    {
        ++m_uiNewUserPv;
        m_uiNewUserVv += oEvent.vv();
        m_ullNewUserSessionLength += oEvent.length();
        if (day_user_iter == m_setDayUser.end())
        {
            ++m_uiNewUserUv;
        }
        oIpEvent.set_user_type(USER_NEW);
    }
    else
    {
        ++m_uiHistoryUserPv;
        m_uiHistoryUserVv += oEvent.vv();
        m_ullHistoryUserSessionLength += oEvent.length();
        if (day_user_iter == m_setDayUser.end())
        {
            ++m_uiHistoryUserUv;
        }
        oIpEvent.set_user_type(USER_HISTORY);
    }
    oMsgBody.set_data(oIpEvent.SerializeAsString());
    oMsgBody.mutable_req_target()->set_route(oIpEvent.client_ip());
    SendOriented("ANALYSE", CMD_USER_IV, GetSequence(), oMsgBody);
}

void SessionUser::AddTourist(const Event& oEvent)
{
    ++m_uiActivityUserPv;
    m_uiActivityUserVv += oEvent.vv();
    m_ullActivityUserSessionLength += oEvent.length();
    ++m_uiTouristPv;
    m_uiTouristVv += oEvent.vv();
    m_ullTouristSessionLength += oEvent.length();
    auto day_user_iter = m_setDayUser.find(oEvent.tourist_id());
    if (day_user_iter == m_setDayUser.end())
    {
        m_setDayUser.insert(oEvent.tourist_id());
        ++m_uiActivityUserUv;
        ++m_uiTouristUv;
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
    --m_uiActivityUserPv;
    m_uiActivityUserVv -= oEvent.vv();
    m_ullActivityUserSessionLength -= oEvent.length();
    --m_uiTouristPv;
    m_uiTouristVv -= oEvent.vv();
    m_ullTouristSessionLength -= oEvent.length();
    auto day_user_iter = m_setDayUser.find(oEvent.tourist_id());
    if (day_user_iter != m_setDayUser.end())
    {
        m_setDayUser.erase(day_user_iter);
        --m_uiActivityUserUv;
        --m_uiTouristUv;
    }
}

void SessionUser::SendResult()
{
    MsgBody oMsgBody;
    Result oResult;
    oMsgBody.mutable_req_target()->set_route_id(m_uiAppId);

    oResult.set_app_id(m_uiAppId);
    oResult.set_channel(m_strChannel);
    oResult.set_tag(m_strTag);

    oResult.set_key5(USER_ACTIVITY);
    oResult.set_pv(m_uiActivityUserPv);
    oResult.set_uv(m_uiActivityUserUv);
    oResult.set_vv(m_uiActivityUserVv);
    oResult.set_length(m_ullActivityUserSessionLength);
    oMsgBody.set_data(oResult.SerializeAsString());
    oMsgBody.mutable_req_target()->set_route_id(m_uiAppId);
    SendOriented("AGGREGATE", CMD_TB_USER, GetSequence(), oMsgBody);

    oResult.set_key5(USER_NEW);
    oResult.set_pv(m_uiNewUserPv);
    oResult.set_uv(m_uiNewUserUv);
    oResult.set_vv(m_uiNewUserVv);
    oResult.set_length(m_ullNewUserSessionLength);
    oMsgBody.set_data(oResult.SerializeAsString());
    oMsgBody.mutable_req_target()->set_route_id(m_uiAppId);
    SendOriented("AGGREGATE", CMD_TB_USER, GetSequence(), oMsgBody);

    oResult.set_key5(USER_HISTORY);
    oResult.set_pv(m_uiHistoryUserPv);
    oResult.set_uv(m_uiHistoryUserUv);
    oResult.set_vv(m_uiHistoryUserVv);
    oResult.set_length(m_ullHistoryUserSessionLength);
    oMsgBody.set_data(oResult.SerializeAsString());
    oMsgBody.mutable_req_target()->set_route_id(m_uiAppId);
    SendOriented("AGGREGATE", CMD_TB_USER, GetSequence(), oMsgBody);

    oResult.set_key5(USER_TOURIST);
    oResult.set_pv(m_uiTouristPv);
    oResult.set_uv(m_uiTouristUv);
    oResult.set_vv(m_uiTouristVv);
    oResult.set_length(m_ullTouristSessionLength);
    oMsgBody.set_data(oResult.SerializeAsString());
    oMsgBody.mutable_req_target()->set_route_id(m_uiAppId);
    SendOriented("AGGREGATE", CMD_TB_USER, GetSequence(), oMsgBody);
}

}
