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
    : AnalyseTimer(strSessionId, dSessionTimeout),
      m_ullStatDate(ullStatDate), m_uiAppId(0), m_strChannel(strChannel), m_strTag(strTag),
      m_iActivityUserIv(0), m_iNewUserIv(0), m_iHistoryUserIv(0), m_iTouristIv(0)
{
}

SessionUserIv::~SessionUserIv()
{
    m_setIp.clear();
}

neb::E_CMD_STATUS SessionUserIv::Timeout()
{
    FlushOut();
    return(neb::CMD_STATUS_RUNNING);
}

void SessionUserIv::AddEvent(const Event& oEvent)
{
    LOG4_TRACE("%s", oEvent.DebugString().c_str());
    if (m_uiAppId == 0)
    {
        m_uiAppId = oEvent.app_id();
    }

    auto ip_iter = m_setIp.find(oEvent.client_ip());
    if (ip_iter == m_setIp.end() && oEvent.client_ip().length() > 0)
    {
        m_setIp.insert(oEvent.client_ip());
        ++m_iActivityUserIv;
        if (USER_TOURIST == oEvent.user_type())
        {
            ++m_iTouristIv;
        }
        else if (USER_NEW == oEvent.user_type())
        {
            ++m_iNewUserIv;
        }
        else if (USER_HISTORY == oEvent.user_type())
        {
            ++m_iHistoryUserIv;
        }
    }
}

void SessionUserIv::FlushOut()
{
    MsgBody oMsgBody;
    Result oResult;
    oMsgBody.mutable_req_target()->set_route_id(m_uiAppId);

    oResult.set_app_id(m_uiAppId);
    oResult.set_channel(m_strChannel);
    oResult.set_tag(m_strTag);

    oResult.set_key5(USER_ACTIVITY);
    oResult.set_iv(m_iActivityUserIv);
    oMsgBody.set_data(oResult.SerializeAsString());
    oMsgBody.mutable_req_target()->set_route_id(m_uiAppId);
    SendOriented("AGGREGATE", CMD_TB_USER, GetSequence(), oMsgBody);

    oResult.set_key5(USER_NEW);
    oResult.set_iv(m_iNewUserIv);
    oMsgBody.set_data(oResult.SerializeAsString());
    oMsgBody.mutable_req_target()->set_route_id(m_uiAppId);
    SendOriented("AGGREGATE", CMD_TB_USER, GetSequence(), oMsgBody);

    oResult.set_key5(USER_HISTORY);
    oResult.set_iv(m_iHistoryUserIv);
    oMsgBody.set_data(oResult.SerializeAsString());
    oMsgBody.mutable_req_target()->set_route_id(m_uiAppId);
    SendOriented("AGGREGATE", CMD_TB_USER, GetSequence(), oMsgBody);

    oResult.set_key5(USER_TOURIST);
    oResult.set_iv(m_iTouristIv);
    oMsgBody.set_data(oResult.SerializeAsString());
    oMsgBody.mutable_req_target()->set_route_id(m_uiAppId);
    SendOriented("AGGREGATE", CMD_TB_USER, GetSequence(), oMsgBody);

    m_iActivityUserIv = 0;
    m_iHistoryUserIv = 0;
    m_iNewUserIv = 0;
    m_iTouristIv = 0;
}

}
