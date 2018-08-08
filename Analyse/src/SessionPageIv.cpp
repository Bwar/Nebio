/*******************************************************************************
 * Project:  Nebio
 * @file     SessionPageIv.cpp
 * @brief    页面统计
 * @author   bwar
 * @date:    2018年4月22日
 * @note     
 * Modify history:
 ******************************************************************************/

#include "SessionPageIv.hpp"

namespace nebio
{

SessionPageIv::SessionPageIv(const std::string& strSessionId,
    const std::string& strChannel, const std::string& strTag, ev_tstamp dSessionTimeout)
    : AnalyseSession(strSessionId, dSessionTimeout),
      m_uiAppId(0), m_strChannel(strChannel), m_strTag(strTag)
{
}

SessionPageIv::~SessionPageIv()
{
    m_setIp.clear();
}

neb::E_CMD_STATUS SessionPageIv::Timeout()
{
    SendResult();
    return(neb::CMD_STATUS_RUNNING);
}

void SessionPageIv::AddEvent(const Event& oEvent)
{
    if (m_strPage.size() == 0)
    {
        m_uiAppId = oEvent.app_id();
        m_strPage = oEvent.page();
    }

    if (oEvent.client_ip().length() > 0)
    {
        m_setIp.insert(oEvent.client_ip());
    }
}

void SessionPageIv::SendResult()
{
    MsgBody oMsgBody;
    Result oResult;
    oResult.set_app_id(m_uiAppId);
    oResult.set_channel(m_strChannel);
    oResult.set_tag(m_strTag);
    oResult.set_key1(m_strPage);
    oResult.set_iv(m_setIp.size());
    oMsgBody.set_data(oResult.SerializeAsString());
    oMsgBody.mutable_req_target()->set_route(m_strPage);
    SendOriented("AGGREGATE", CMD_TB_PAGE, GetSequence(), oMsgBody);
}

}
