/*******************************************************************************
 * Project:  Nebio
 * @file     SessionTbPage.cpp
 * @brief    页面统计结果
 * @author   bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/

#include <mydis/DbOperator.hpp>
#include "UnixTime.hpp"
#include "SessionTbPage.hpp"
#include "StepWriteDb.hpp"

namespace nebio
{

SessionTbPage::SessionTbPage(const std::string& strSessionId, ev_tstamp dSessionTimeout)
    : AggregateSession(strSessionId, dSessionTimeout),
      m_uiAppId(0), m_uiUv(0), m_uiPv(0), m_uiVv(0), m_uiIv(0),
      m_uiExitVv(0), m_uiBounceVv(0), m_ullOnlineTime(0)
{
}

SessionTbPage::~SessionTbPage()
{
}

neb::E_CMD_STATUS SessionTbPage::Timeout()
{
    FlushOut();
    return(neb::CMD_STATUS_RUNNING);
}

void SessionTbPage::AddResult(const Result& oResult)
{
    LOG4_DEBUG("%s", oResult.DebugString().c_str());
    if (0 == m_uiAppId)
    {
        m_uiAppId = oResult.app_id();
        m_strChannel = oResult.channel();
        m_strTag = oResult.tag();
        m_strPage = oResult.key1();
    }
    m_uiUv += oResult.uv();
    m_uiPv += oResult.pv();
    m_uiVv += oResult.vv();
    m_uiIv += oResult.iv();
    m_uiExitVv += oResult.exit_vv();
    m_uiBounceVv += oResult.bounce_vv();
    m_ullOnlineTime += oResult.length();
}

void SessionTbPage::FlushOut()
{
    neb::DbOperator oDbOper(1, "tb_page", neb::Mydis::DbOperate::REPLACE, 1);
    oDbOper.AddDbField("stat_date", neb::time_t2TimeStr((time_t)GetNowTime(), "%Y-%m-%d"));
    oDbOper.AddDbField("app_id", m_uiAppId);
    oDbOper.AddDbField("channel", m_strChannel);
    oDbOper.AddDbField("tag", m_strTag);
    oDbOper.AddDbField("page", m_strPage);
    oDbOper.AddDbField("uv", m_uiUv);
    oDbOper.AddDbField("pv", m_uiPv);
    oDbOper.AddDbField("vv", m_uiVv);
    oDbOper.AddDbField("iv", m_uiIv);
    oDbOper.AddDbField("online_time", m_ullOnlineTime);
    oDbOper.AddDbField("exit_vv", m_uiExitVv);
    oDbOper.AddDbField("bounce_vv", m_uiBounceVv);
    LOG4_DEBUG("%s", oDbOper.MakeMemOperate()->DebugString().c_str());
    auto pStep = MakeSharedStep("nebio::StepWriteDb");
    pStep->Emit(neb::ERR_OK, "", (void*)(oDbOper.MakeMemOperate()));
    m_uiUv = 0;
    m_uiPv = 0;
    m_uiVv = 0;
    m_uiIv = 0;
    m_uiExitVv = 0;
    m_uiBounceVv = 0;
    m_ullOnlineTime = 0;
}

}
