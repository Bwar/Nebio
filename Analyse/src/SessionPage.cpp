/*******************************************************************************
 * Project:  Nebio
 * @file     SessionPage.cpp
 * @brief    页面统计
 * @author   bwar
 * @date:    2018年4月22日
 * @note     
 * Modify history:
 ******************************************************************************/

#include "SessionPage.hpp"

namespace nebio
{

SessionPage::SessionPage(const std::string& strSessionId,
    const std::string& strChannel, const std::string& strTag, ev_tstamp dSessionTimeout)
    : AnalyseTimer(strSessionId, dSessionTimeout),
      m_uiAppId(0), m_strChannel(strChannel), m_strTag(strTag), m_iPv(0), m_iUv(0), m_iVv(0),
      m_iExitVv(0), m_iBounceVv(0), m_llPageLength(0)
{
}

SessionPage::~SessionPage()
{
    m_setUserId.clear();
}

neb::E_CMD_STATUS SessionPage::Timeout()
{
    FlushOut();
    return(neb::CMD_STATUS_RUNNING);
}

void SessionPage::AddEvent(const Event& oEvent)
{
    LOG4_TRACE("%s", oEvent.DebugString().c_str());
    if (m_strPage.size() == 0)
    {
        m_uiAppId = oEvent.app_id();
        m_strPage = oEvent.page();
    }

    if (Event::EVENT_ADD == oEvent.event_oper())
    {
        ++m_iPv;
        m_iVv += oEvent.vv();
        m_iExitVv += oEvent.exit_vv();
        m_iBounceVv += oEvent.bounce_vv();
        m_llPageLength += oEvent.length();

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
        m_iExitVv -= oEvent.exit_vv();
        m_iBounceVv -= oEvent.bounce_vv();
        m_llPageLength -= oEvent.length();

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

void SessionPage::FlushOut()
{
    MsgBody oMsgBody;
    Result oResult;
    oResult.set_app_id(m_uiAppId);
    oResult.set_channel(m_strChannel);
    oResult.set_tag(m_strTag);
    oResult.set_key1(m_strPage);
    oResult.set_pv(m_iPv);
    oResult.set_uv(m_iUv);
    oResult.set_vv(m_iVv);
    oResult.set_length(m_llPageLength);
    oResult.set_exit_vv(m_iExitVv);
    oResult.set_bounce_vv(m_iBounceVv);
    oMsgBody.set_data(oResult.SerializeAsString());
    oMsgBody.mutable_req_target()->set_route(m_strPage);
    SendOriented("AGGREGATE", CMD_TB_PAGE, GetSequence(), oMsgBody);
    m_iPv = 0;
    m_iUv = 0;
    m_iVv = 0;
    m_iExitVv = 0;
    m_iBounceVv = 0;
    m_llPageLength = 0;
}

}
