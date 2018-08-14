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
#include "UnixTime.hpp"

namespace nebio
{

SessionEvent::SessionEvent(const std::string& strSessionId,
    const std::string& strChannel, const std::string& strTag, uint32 uiDate, ev_tstamp dSessionTimeout)
    : AnalyseTimer(strSessionId, dSessionTimeout),
      m_uiDate(uiDate), m_uiAppId(0), m_strChannel(strChannel), m_strTag(strTag), m_iPv(0), m_iUv(0), m_iVv(0), m_llEventLength(0)
{
}

SessionEvent::~SessionEvent()
{
    m_setUserId.clear();
}

neb::E_CMD_STATUS SessionEvent::Timeout()
{
    FlushOut();
    uint32 uiDate = std::stoul(neb::time_t2TimeStr((time_t)GetNowTime(), "%Y%m%d"));
    if (uiDate > m_uiDate)
    {
        /*
        m_uiDate = uiDate;
        m_setUserId.clear();
        */
        return(neb::CMD_STATUS_COMPLETED);
    }
    return(neb::CMD_STATUS_RUNNING);
}

void SessionEvent::AddEvent(const Event& oEvent)
{
    LOG4_TRACE("%s", oEvent.DebugString().c_str());
    if (m_strEventId.size() == 0)
    {
        m_uiAppId = oEvent.app_id();
        m_strEventId = oEvent.event_id();
        m_strEventType = oEvent.event_type();
    }

    if (Event::EVENT_ADD == oEvent.event_oper())
    {
        ++m_iPv;
        m_iVv += oEvent.vv();
        m_llEventLength += oEvent.length();

        if (oEvent.user_id().length() > 0)
        {
            auto it = m_setUserId.find(oEvent.user_id());
            if (it == m_setUserId.end())
            {
                ++m_iUv;
                m_setUserId.insert(oEvent.user_id());
            }
        }
        else if (oEvent.tourist_id().length() > 0)
        {
            auto it = m_setUserId.find(oEvent.tourist_id());
            if (it == m_setUserId.end())
            {
                ++m_iUv;
                m_setUserId.insert(oEvent.tourist_id());
            }
        }
    }
    else
    {
        --m_iPv;
        m_iVv -= oEvent.vv();
        m_llEventLength -= oEvent.length();

        if (oEvent.tourist_id().length() > 0)
        {
            auto it = m_setUserId.find(oEvent.tourist_id());
            if (it != m_setUserId.end())
            {
                --m_iUv;
                m_setUserId.erase(it);
            }
        }
        else if (oEvent.user_id().length() > 0)
        {
            auto it = m_setUserId.find(oEvent.user_id());
            if (it != m_setUserId.end())
            {
                --m_iUv;
                m_setUserId.erase(it);
            }
        }
    }
}

void SessionEvent::FlushOut()
{
    MsgBody oMsgBody;
    Result oResult;
    oResult.set_app_id(m_uiAppId);
    oResult.set_channel(m_strChannel);
    oResult.set_tag(m_strTag);
    oResult.set_key1(m_strEventId);
    oResult.set_pv(m_iPv);
    oResult.set_uv(m_iUv);
    oResult.set_vv(m_iVv);
    oResult.set_length(m_llEventLength);
    oMsgBody.set_data(oResult.SerializeAsString());
    oMsgBody.mutable_req_target()->set_route(m_strEventId);
    SendOriented("AGGREGATE", CMD_TB_EVENT, GetSequence(), oMsgBody);
    m_iPv = 0;
    m_iUv = 0;
    m_iVv = 0;
    m_llEventLength = 0;
}

}
