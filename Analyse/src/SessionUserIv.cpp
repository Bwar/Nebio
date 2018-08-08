/*******************************************************************************
 * Project:  Nebio
 * @file     SessionUserIv.cpp
 * @brief    事件统计
 * @author   bwar
 * @date:    2018年4月22日
 * @note     
 * Modify history:
 ******************************************************************************/

#include "SessionUser.hpp"
#include "SessionUserIv.hpp"

namespace nebio
{

SessionUserIv::SessionUserIv(const std::string& strSessionId,
    const std::string& strChannel, const std::string& strTag, uint64 ullStatDate, ev_tstamp dSessionTimeout)
    : AnalyseSession(strSessionId, dSessionTimeout),
      m_ullStatDate(ullStatDate), m_uiAppId(0), m_strChannel(strChannel), m_strTag(strTag),
      m_uiActivityUserIv(0), m_uiNewUserIv(0), m_uiHistoryUserIv(0), m_uiTouristIv(0)
{
}

SessionUserIv::~SessionUserIv()
{
    m_setIp.clear();
}

neb::E_CMD_STATUS SessionUserIv::Timeout()
{
    SendResult();
    return(neb::CMD_STATUS_RUNNING);
}

void SessionUserIv::AddEvent(const Event& oEvent)
{
    if (m_uiAppId == 0)
    {
        m_uiAppId = oEvent.app_id();
    }

    auto ip_iter = m_setIp.find(oEvent.client_ip());
    if (ip_iter == m_setIp.end() && oEvent.client_ip().length() > 0)
    {
        m_setIp.insert(oEvent.client_ip());
        ++m_uiActivityUserIv;
        if (USER_TOURIST == oEvent.user_type())
        {
            ++m_uiTouristIv;
        }
        else if (USER_NEW == oEvent.user_type())
        {
            ++m_uiNewUserIv;
        }
        else if (USER_HISTORY == oEvent.user_type())
        {
            ++m_uiHistoryUserIv;
        }
    }
}

void SessionUserIv::SendResult()
{
    MsgBody oMsgBody;
    Result oResult;
    oMsgBody.mutable_req_target()->set_route_id(m_uiAppId);

    oResult.set_app_id(m_uiAppId);
    oResult.set_channel(m_strChannel);
    oResult.set_tag(m_strTag);

    oResult.set_key5(USER_ACTIVITY);
    oResult.set_iv(m_uiActivityUserIv);
    oMsgBody.set_data(oResult.SerializeAsString());
    oMsgBody.mutable_req_target()->set_route_id(m_uiAppId);
    SendOriented("AGGREGATE", CMD_TB_USER, GetSequence(), oMsgBody);

    oResult.set_key5(USER_NEW);
    oResult.set_iv(m_uiNewUserIv);
    oMsgBody.set_data(oResult.SerializeAsString());
    oMsgBody.mutable_req_target()->set_route_id(m_uiAppId);
    SendOriented("AGGREGATE", CMD_TB_USER, GetSequence(), oMsgBody);

    oResult.set_key5(USER_HISTORY);
    oResult.set_iv(m_uiHistoryUserIv);
    oMsgBody.set_data(oResult.SerializeAsString());
    oMsgBody.mutable_req_target()->set_route_id(m_uiAppId);
    SendOriented("AGGREGATE", CMD_TB_USER, GetSequence(), oMsgBody);

    oResult.set_key5(USER_TOURIST);
    oResult.set_iv(m_uiTouristIv);
    oMsgBody.set_data(oResult.SerializeAsString());
    oMsgBody.mutable_req_target()->set_route_id(m_uiAppId);
    SendOriented("AGGREGATE", CMD_TB_USER, GetSequence(), oMsgBody);
}

}
