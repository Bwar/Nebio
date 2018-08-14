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

SessionTbPage::SessionTbPage(const std::string& strSessionId,
        uint32 uiDate, const std::string& strDate, ev_tstamp dSessionTimeout)
    : AggregateTimer(strSessionId, dSessionTimeout),
      m_uiDate(uiDate), m_strDate(strDate),
      m_uiAppId(0), m_iUv(0), m_iPv(0), m_iVv(0), m_iIv(0),
      m_iExitVv(0), m_iBounceVv(0), m_llOnlineTime(0)
{
}

SessionTbPage::~SessionTbPage()
{
}

neb::E_CMD_STATUS SessionTbPage::Timeout()
{
    WriteResult();
    uint32 uiDate = std::stoul(neb::time_t2TimeStr((time_t)GetNowTime(), "%Y%m%d"));
    if (uiDate > m_uiDate)
    {
        /*
        m_uiDate = uiDate;
        m_strDate = neb::time_t2TimeStr((time_t)GetNowTime(), "%Y-%m-%d");
        m_iUv = 0;
        m_iPv = 0;
        m_iVv = 0;
        m_iIv = 0;
        m_iExitVv = 0;
        m_iBounceVv = 0;
        m_llOnlineTime = 0;
        */
        return(neb::CMD_STATUS_COMPLETED);
    }
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
    m_iUv += oResult.uv();
    m_iPv += oResult.pv();
    m_iVv += oResult.vv();
    m_iIv += oResult.iv();
    m_iExitVv += oResult.exit_vv();
    m_iBounceVv += oResult.bounce_vv();
    m_llOnlineTime += oResult.length();
}

void SessionTbPage::WriteResult()
{
    neb::DbOperator oDbOper(1, "tb_page", neb::Mydis::DbOperate::REPLACE, 1);
    oDbOper.AddDbField("stat_date", m_strDate);
    oDbOper.AddDbField("app_id", m_uiAppId);
    oDbOper.AddDbField("channel", m_strChannel);
    oDbOper.AddDbField("tag", m_strTag);
    oDbOper.AddDbField("page", m_strPage);
    oDbOper.AddDbField("uv", m_iUv);
    oDbOper.AddDbField("pv", m_iPv);
    oDbOper.AddDbField("vv", m_iVv);
    oDbOper.AddDbField("iv", m_iIv);
    oDbOper.AddDbField("online_time", m_llOnlineTime);
    oDbOper.AddDbField("exit_vv", m_iExitVv);
    oDbOper.AddDbField("bounce_vv", m_iBounceVv);
    LOG4_DEBUG("%s", oDbOper.MakeMemOperate()->DebugString().c_str());
    auto pStep = MakeSharedStep("nebio::StepWriteDb");
    pStep->Emit(neb::ERR_OK, "", (void*)(oDbOper.MakeMemOperate()));
}

}
