/*******************************************************************************
 * Project:  Nebio
 * @file     SessionEvent.cpp
 * @brief    事件统计
 * @author   bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/

#include "SessionEvent.hpp"

namespace nebio
{

SessionEvent::SessionEvent(const std::string& strSessionId,
    const std::string& strChannel, const std::string& strTag, ev_tstamp dSessionTimeout)
    : AnalyseSession(strSessionId, dSessionTimeout),
      m_uiAppId(0), m_strChannel(strChannel), m_strTag(strTag), m_uiPv(0), m_uiVv(0), m_ullEventLength(0)
{
}

SessionEvent::~SessionEvent()
{
    m_setUserId.clear();
}

neb::E_CMD_STATUS SessionEvent::Timeout()
{
    SendResult();
    return(neb::CMD_STATUS_RUNNING);
}

void SessionEvent::AddEvent(const Event& oEvent)
{
    if (m_strEventId.size() == 0)
    {
        m_uiAppId = oEvent.app_id();
        m_strEventId = oEvent.event_id();
        m_strEventType = oEvent.event_type();
    }

    if (Event::EVENT_ADD == oEvent.event_oper())
    {
        ++m_uiPv;
        m_uiVv += oEvent.vv();
        m_ullEventLength += oEvent.length();

        if (oEvent.user_id().length() > 0)
        {
            m_setUserId.insert(oEvent.user_id());
        }
        else if (oEvent.tourist_id().length() > 0)
        {
            m_setUserId.insert(oEvent.tourist_id());
        }
    }
    else
    {
        --m_uiPv;
        m_uiVv -= oEvent.vv();
        m_ullEventLength -= oEvent.length();

        if (oEvent.tourist_id().length() > 0)
        {
            auto it = m_setUserId.find(oEvent.tourist_id());
            if (it != m_setUserId.end())
            {
                m_setUserId.erase(it);
            }
        }
        else if (oEvent.user_id().length() > 0)
        {
            auto it = m_setUserId.find(oEvent.user_id());
            if (it != m_setUserId.end())
            {
                m_setUserId.erase(it);
            }
        }
    }
}

void SessionEvent::SendResult()
{
    MsgBody oMsgBody;
    Result oResult;
    oResult.set_app_id(m_uiAppId);
    oResult.set_channel(m_strChannel);
    oResult.set_tag(m_strTag);
    oResult.set_key1(m_strEventId);
    oResult.set_pv(m_uiPv);
    oResult.set_uv(m_setUserId.size());
    oResult.set_vv(m_uiVv);
    oResult.set_length(m_ullEventLength);
    oMsgBody.set_data(oResult.SerializeAsString());
    oMsgBody.mutable_req_target()->set_route(m_strEventId);
    SendOriented("AGGREGATE", CMD_TB_EVENT, GetSequence(), oMsgBody);
}

}
