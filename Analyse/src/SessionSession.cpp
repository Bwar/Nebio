/*******************************************************************************
 * Project:  Nebio
 * @file     SessionSession.cpp
 * @brief    会话统计
 * @author   Bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/

#include "SessionSession.hpp"

namespace nebio
{

SessionSession::SessionSession(const std::string& strSessionId, ev_tstamp dSessionTimeout)
    : AnalyseSession(strSessionId, dSessionTimeout),
      m_bTourist2User(false), m_uiAppId(0)
{
}

SessionSession::~SessionSession()
{
    m_listEvent.clear();
    m_listPage.clear();
    m_setEventVv.clear();
    m_setPageVv.clear();
}

neb::E_CMD_STATUS SessionSession::Timeout()
{
    // TODO add session stat
    TransferEvent(-1);
    TransferPageEvent(-1);
    return(neb::CMD_STATUS_COMPLETED);
}

void SessionSession::AddEvent(const Event& oEvent)
{
    if (m_strUserSessionId.length() == 0)
    {
        m_uiAppId = oEvent.app_id();
        m_strUserSessionId = oEvent.session_id();
        m_strDeviceId = oEvent.device_id();
        m_strPlat = oEvent.plat();
        m_strExplorer = oEvent.explorer();
        m_strClientIp = oEvent.client_ip();
        if (oEvent.referer().length() == 0)
        {
            neb::CJsonObject oJsonConf = GetCustomConf();
            m_strReferer = oJsonConf["analyse"]("direct_access");
        }
        else
        {
            m_strReferer = oEvent.referer();
        }
    }
    if (m_strUserId.length() == 0 && oEvent.user_id().length() < 30 && oEvent.user_id().length() > 1)    // register user id
    {
        m_strUserId = oEvent.user_id();
        if (m_strTouristId.length() > 0)
        {
            m_bTourist2User = true;
        }
    }
    if (m_strTouristId.length() == 0 && oEvent.user_id().length() >= 30)     // tourist id is a GUID, length 36.
    {
        m_strTouristId = oEvent.user_id();
    }

    m_listEvent.push_back(std::move(std::make_tuple(oEvent.event_id(), oEvent.event_type(),
                oEvent.time(), oEvent.page())));
    if (m_listEvent.size() > 1)
    {
        TransferEvent(-2);
    }

    if (oEvent.event_type() == "page")
    {
        m_listPage.push_back(std::move(std::make_tuple(oEvent.page(), oEvent.time())));
        if (m_listPage.size() > 1)
        {
            TransferPageEvent(-2);
        }
    }

    m_bTourist2User = false;
}

void SessionSession::TransferEvent(int iEventPos)
{
    LOG4_TRACE("iEventPos = %d", iEventPos);
    MsgBody oMsgBody;
    Event oEvent;

    auto r_iter = m_listEvent.rbegin();
    if (iEventPos == -2)
    {
        ++r_iter;
        if (r_iter == m_listEvent.rend())
        {
            return;
        }
        oEvent.set_length(std::get<2>(m_listEvent.back()) - std::get<2>(*r_iter));
    }
    if (iEventPos == -1)
    {
        oEvent.set_length(std::get<2>(m_listEvent.back()) - std::get<2>(m_listEvent.front()));
    }
    oEvent.set_event_id(std::get<0>(*r_iter));
    oEvent.set_event_type(std::get<1>(*r_iter));
    oEvent.set_time(std::get<2>(*r_iter));
    oEvent.set_page(std::get<3>(*r_iter));
    oEvent.set_referer(m_strReferer);
    oEvent.set_session_id(m_strUserSessionId);
    oEvent.set_user_id(m_strUserId);
    oEvent.set_device_id(m_strDeviceId);
    oEvent.set_plat(m_strPlat);
    oEvent.set_explorer(m_strExplorer);
    oEvent.set_client_ip(m_strClientIp);
    oEvent.set_app_id(m_uiAppId);
    oEvent.set_tourist_id(m_strTouristId);

    if (m_setEventVv.end() == m_setEventVv.find(oEvent.event_id()))
    {
        oEvent.set_vv(1);
        m_setEventVv.insert(oEvent.event_id());
    }

    if (m_bTourist2User)
    {
        oEvent.set_event_oper(Event::EVENT_DEL);
        oMsgBody.set_data(oEvent.SerializeAsString());
        oMsgBody.mutable_req_target()->set_route(m_strTouristId);
        SendOriented("ANALYSE", CMD_EVENT, GetSequence(), oMsgBody);
        SendOriented("ANALYSE", CMD_USER, GetSequence(), oMsgBody);
    }

    oEvent.set_event_oper(Event::EVENT_ADD);
    oMsgBody.set_data(oEvent.SerializeAsString());
    if (oEvent.user_id().length() > 0)                // 当且仅当游客身份转为注册用户身份时明确以游客id路由
    {
        oMsgBody.mutable_req_target()->set_route(m_strUserId);
    }
    else
    {
        oMsgBody.mutable_req_target()->set_route(m_strTouristId);
    }
    SendOriented("ANALYSE", CMD_EVENT, GetSequence(), oMsgBody);
    SendOriented("ANALYSE", CMD_USER, GetSequence(), oMsgBody);

    if (m_listEvent.size() == 1)
    {
        oMsgBody.mutable_req_target()->set_route(m_strClientIp);
        SendOriented("ANALYSE", CMD_EVENT_IV, GetSequence(), oMsgBody);
    }
}

void SessionSession::TransferPageEvent(int iEventPos)
{
    LOG4_TRACE("iEventPos = %d", iEventPos);
    MsgBody oMsgBody;
    Event oEvent;

    auto r_iter = m_listPage.rbegin();
    if (iEventPos == -2)
    {
        ++r_iter;
        if (r_iter == m_listPage.rend())
        {
            return;
        }
        oEvent.set_next_page(std::get<0>(m_listPage.back()));
        oEvent.set_length(std::get<1>(m_listPage.back()) - std::get<1>(*r_iter));
    }
    oEvent.set_page(std::get<0>(*r_iter));
    oEvent.set_time(std::get<1>(*r_iter));
    oEvent.set_referer(m_strReferer);
    oEvent.set_session_id(m_strUserSessionId);
    oEvent.set_user_id(m_strUserId);
    oEvent.set_device_id(m_strDeviceId);
    oEvent.set_plat(m_strPlat);
    oEvent.set_explorer(m_strExplorer);
    oEvent.set_client_ip(m_strClientIp);
    oEvent.set_app_id(m_uiAppId);
    oEvent.set_tourist_id(m_strTouristId);

    if (m_setPageVv.end() == m_setPageVv.find(oEvent.page()))
    {
        oEvent.set_vv(1);
        m_setPageVv.insert(oEvent.page());
    }
    if (iEventPos == -1)
    {
        oEvent.set_exit_vv(1);
        if (m_listPage.size() == 1)
        {
            oEvent.set_bounce_vv(1);
        }
    }

    if (m_bTourist2User)
    {
        oEvent.set_event_oper(Event::EVENT_DEL);
        oMsgBody.set_data(oEvent.SerializeAsString());
        oMsgBody.mutable_req_target()->set_route(m_strTouristId);
        SendOriented("ANALYSE", CMD_PAGE, GetSequence(), oMsgBody);
    }

    oEvent.set_event_oper(Event::EVENT_ADD);
    oMsgBody.set_data(oEvent.SerializeAsString());
    if (m_strUserId.length() > 0)
    {
        oMsgBody.mutable_req_target()->set_route(m_strUserId);
    }
    else
    {
        oMsgBody.mutable_req_target()->set_route(m_strTouristId);
    }
    SendOriented("ANALYSE", CMD_PAGE, GetSequence(), oMsgBody);
    if (m_listPage.size() == 1)
    {
        oMsgBody.mutable_req_target()->set_route(m_strClientIp);
        SendOriented("ANALYSE", CMD_PAGE_IV, GetSequence(), oMsgBody);
    }
}

}
