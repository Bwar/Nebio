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
    : AnalyseSession(strSessionId, dSessionTimeout, "nebio::SessionUser"),
      m_ullStatDate(ullStatDate), m_uiAppId(0), m_strChannel(strChannel), m_strTag(strTag),
      m_uiActivityUserUv(0), m_uiActivityUserPv(0), m_uiActivityUserVv(0), m_uiActivityUserIv(0),
      m_uiNewUserUv(0), m_uiNewUserPv(0), m_uiNewUserVv(0), m_uiNewUserIv(0),
      m_uiHistoryUserUv(0), m_uiHistoryUserPv(0), m_uiHistoryUserVv(0), m_uiHistoryUserIv(0),
      m_uiTouristUv(0), m_uiTouristPv(0), m_uiTouristVv(0), m_uiTouristIv(0)
{
}

SessionUser::~SessionUser()
{
    m_mapUser.clear();
    m_setIp.clear();
}

neb::E_CMD_STATUS SessionUser::Timeout()
{
    Stat();
    return(neb::CMD_STATUS_RUNNING);
}

void SessionUser::AddEvent(const Event& oEvent)
{
    if (m_uiAppId == 0)
    {
        m_uiAppId = oEvent.app_id();
    }

    if (oEvent.user_id().length() > 0)
    {
        if (oEvent.tourist_id().length() > 0)       // 游客转为注册用户（oEvent.user_id().length() > 0 && oEvent.tourist_id().length() > 0）
        {
            DelTourist(oEvent);
            TransferUser(oEvent);
        }
        else
        {
            AddUser(oEvent);
        }
    }
    else
    {
        AddTourist(oEvent);
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
    m_ullActivityUserSessionLength += oEvent.session_length();
    auto day_user_iter = m_setDayUser.find(oEvent.user_id());
    if (day_user_iter == m_setDayUser.end())
    {
        m_setDayUser.insert(oEvent.user_id());
        ++m_uiActivityUserUv;
    }
    auto ip_iter = m_setIp.find(oEvent.client_ip());
    if (ip_iter == m_setIp.end())
    {
        m_setIp.insert(oEvent.client_ip());
        ++m_uiActivityUserIv;
    }

    if (bIsNewUser)
    {
        ++m_uiNewUserPv;
        m_uiNewUserVv += oEvent.vv();
        m_ullNewUserSessionLength += oEvent.session_length();
        if (day_user_iter == m_setDayUser.end())
        {
            ++m_uiNewUserUv;
        }
        if (ip_iter == m_setIp.end())
        {
            ++m_uiNewUserIv;
        }
    }
    else
    {
        ++m_uiHistoryUserPv;
        m_uiHistoryUserVv += oEvent.vv();
        m_ullHistoryUserSessionLength += oEvent.session_length();
        if (day_user_iter == m_setDayUser.end())
        {
            ++m_uiHistoryUserUv;
        }
        if (ip_iter == m_setIp.end())
        {
            ++m_uiHistoryUserIv;
        }
    }
}

void SessionUser::AddTourist(const Event& oEvent)
{
    ++m_uiActivityUserPv;
    m_uiActivityUserVv += oEvent.vv();
    m_ullActivityUserSessionLength += oEvent.session_length();
    ++m_uiTouristPv;
    m_uiTouristVv += oEvent.vv();
    m_ullTouristSessionLength += oEvent.session_length();
    auto day_user_iter = m_setDayUser.find(oEvent.tourist_id());
    if (day_user_iter == m_setDayUser.end())
    {
        m_setDayUser.insert(oEvent.tourist_id());
        ++m_uiActivityUserUv;
        ++m_uiTouristUv;
    }
    auto ip_iter = m_setIp.find(oEvent.client_ip());
    if (ip_iter == m_setIp.end())
    {
        m_setIp.insert(oEvent.client_ip());
        ++m_uiActivityUserIv;
        ++m_uiTouristIv;
    }
}

void SessionUser::DelTourist(const Event& oEvent)
{
    --m_uiActivityUserPv;
    --m_uiTouristPv;
    auto day_user_iter = m_setDayUser.find(oEvent.tourist_id());
    if (day_user_iter != m_setDayUser.end())
    {
        m_setDayUser.erase(day_user_iter);
        --m_uiActivityUserUv;
        --m_uiTouristUv;
    }
    auto ip_iter = m_setIp.find(oEvent.client_ip());
    if (ip_iter != m_setIp.end())
    {
        m_setIp.erase(ip_iter);
        --m_uiActivityUserIv;
        --m_uiTouristIv;
    }
}

void SessionUser::TransferUser(const Event& oEvent)
{
    MsgBody oMsgBody;
    Event oOutEvent = oEvent;
    oOutEvent.set_tourist_id("");
    oMsgBody.set_data(oOutEvent.SerializeAsString());
    oMsgBody.mutable_req_target()->set_route(oEvent.user_id());
    SendOriented("ANALYSE", CMD_USER, GetSequence(), oMsgBody);
}

void SessionUser::Stat()
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
    oResult.set_iv(m_uiActivityUserIv);
    oResult.set_length(m_ullActivityUserSessionLength);
    oMsgBody.set_data(oResult.SerializeAsString());
    SendOriented("AGGREGATE", CMD_TB_USER, GetSequence(), oMsgBody);

    oResult.set_key5(USER_NEW);
    oResult.set_pv(m_uiNewUserPv);
    oResult.set_uv(m_uiNewUserUv);
    oResult.set_vv(m_uiNewUserVv);
    oResult.set_iv(m_uiNewUserIv);
    oResult.set_length(m_ullNewUserSessionLength);
    oMsgBody.set_data(oResult.SerializeAsString());
    SendOriented("AGGREGATE", CMD_TB_USER, GetSequence(), oMsgBody);

    oResult.set_key5(USER_HISTORY);
    oResult.set_pv(m_uiHistoryUserPv);
    oResult.set_uv(m_uiHistoryUserUv);
    oResult.set_vv(m_uiHistoryUserVv);
    oResult.set_iv(m_uiHistoryUserIv);
    oResult.set_length(m_ullHistoryUserSessionLength);
    oMsgBody.set_data(oResult.SerializeAsString());
    SendOriented("AGGREGATE", CMD_TB_USER, GetSequence(), oMsgBody);

    oResult.set_key5(USER_TOURIST);
    oResult.set_pv(m_uiTouristPv);
    oResult.set_uv(m_uiTouristUv);
    oResult.set_vv(m_uiTouristVv);
    oResult.set_iv(m_uiTouristIv);
    oResult.set_length(m_ullTouristSessionLength);
    oMsgBody.set_data(oResult.SerializeAsString());
    SendOriented("AGGREGATE", CMD_TB_USER, GetSequence(), oMsgBody);
}

}
