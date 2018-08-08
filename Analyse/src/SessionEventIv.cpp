/*******************************************************************************
 * Project:  Nebio
 * @file     SessionEventIv.cpp
 * @brief    事件IP数统计
 * @author   bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/

#include "SessionEventIv.hpp"

namespace nebio
{

SessionEventIv::SessionEventIv(const std::string& strSessionId,
    const std::string& strChannel, const std::string& strTag, ev_tstamp dSessionTimeout)
    : AnalyseSession(strSessionId, dSessionTimeout),
      m_uiAppId(0), m_strChannel(strChannel), m_strTag(strTag), m_iIv(0)
{
}

SessionEventIv::~SessionEventIv()
{
    m_setIp.clear();
}

neb::E_CMD_STATUS SessionEventIv::Timeout()
{
    FlushOut();
    return(neb::CMD_STATUS_RUNNING);
}

void SessionEventIv::AddEvent(const Event& oEvent)
{
    LOG4_TRACE("%s", oEvent.DebugString().c_str());
    if (m_strEventId.size() == 0)
    {
        m_uiAppId = oEvent.app_id();
        m_strEventId = oEvent.event_id();
        m_strEventType = oEvent.event_type();
        m_strPage = oEvent.page();
    }

    if (oEvent.client_ip().length() > 0)
    {
        auto it = m_setIp.find(oEvent.client_ip());
        if (it == m_setIp.end())
        {
            ++m_iIv;
            m_setIp.insert(oEvent.client_ip());
        }
    }
}

void SessionEventIv::FlushOut()
{
    MsgBody oMsgBody;
    Result oResult;
    oResult.set_app_id(m_uiAppId);
    oResult.set_channel(m_strChannel);
    oResult.set_tag(m_strTag);
    oResult.set_key1(m_strEventId);
    oResult.set_iv(m_iIv);
    oMsgBody.set_data(oResult.SerializeAsString());
    oMsgBody.mutable_req_target()->set_route(m_strEventId);
    SendOriented("AGGREGATE", CMD_TB_EVENT, GetSequence(), oMsgBody);
    m_iIv = 0;
}

}
