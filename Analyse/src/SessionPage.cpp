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
    : AnalyseSession(strSessionId, dSessionTimeout),
      m_uiAppId(0), m_strChannel(strChannel), m_strTag(strTag), m_uiPv(0), m_uiVv(0),
      m_uiExitVv(0), m_uiBounceVv(0), m_ullPageLength(0)
{
}

SessionPage::~SessionPage()
{
    m_setUserId.clear();
}

neb::E_CMD_STATUS SessionPage::Timeout()
{
    SendResult();
    return(neb::CMD_STATUS_RUNNING);
}

void SessionPage::AddEvent(const Event& oEvent)
{
    if (m_strPage.size() == 0)
    {
        m_uiAppId = oEvent.app_id();
        m_strPage = oEvent.page();
    }

    if (Event::EVENT_ADD == oEvent.event_oper())
    {
        ++m_uiPv;
        m_uiVv += oEvent.vv();
        m_uiExitVv += oEvent.exit_vv();
        m_uiBounceVv += oEvent.bounce_vv();
        m_ullPageLength += oEvent.length();

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
        m_uiExitVv -= oEvent.exit_vv();
        m_uiBounceVv -= oEvent.bounce_vv();
        m_ullPageLength -= oEvent.length();

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

void SessionPage::SendResult()
{
    MsgBody oMsgBody;
    Result oResult;
    oResult.set_app_id(m_uiAppId);
    oResult.set_channel(m_strChannel);
    oResult.set_tag(m_strTag);
    oResult.set_key1(m_strPage);
    oResult.set_pv(m_uiPv);
    oResult.set_uv(m_setUserId.size());
    oResult.set_vv(m_uiVv);
    oResult.set_length(m_ullPageLength);
    oResult.set_exit_vv(m_uiExitVv);
    oResult.set_bounce_vv(m_uiBounceVv);
    oMsgBody.set_data(oResult.SerializeAsString());
    oMsgBody.mutable_req_target()->set_route(m_strPage);
    SendOriented("AGGREGATE", CMD_TB_PAGE, GetSequence(), oMsgBody);
}

}
