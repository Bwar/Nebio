/*******************************************************************************
 * Project:  Nebio
 * @file     SessionTbEvent.cpp
 * @brief    事件统计
 * @author   bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/

#include <mydis/DbOperator.hpp>
#include "UnixTime.hpp"
#include "SessionTbEvent.hpp"
#include "StepWriteDb.hpp"

namespace nebio
{

SessionTbEvent::SessionTbEvent(const std::string& strSessionId,
        uint32 uiDate, const std::string& strDate, ev_tstamp dSessionTimeout)
    : AggregateTimer(strSessionId, dSessionTimeout),
      m_uiDate(uiDate), m_strDate(strDate),
      m_uiAppId(0), m_iUv(0), m_iPv(0), m_iVv(0), m_iIv(0), m_llEventLength(0)
{
}

SessionTbEvent::~SessionTbEvent()
{
}

neb::E_CMD_STATUS SessionTbEvent::Timeout()
{
    WriteResult();
    uint32 uiDate = std::stoul(neb::time_t2TimeStr((time_t)GetNowTime(), "%Y%m%d"));
    if (uiDate > m_uiDate)
    {
        m_uiDate = uiDate;
        m_strDate = neb::time_t2TimeStr((time_t)GetNowTime(), "%Y-%m-%d");
        m_iUv = 0;
        m_iPv = 0;
        m_iVv = 0;
        m_iIv = 0;
        m_llEventLength = 0;
    }
    return(neb::CMD_STATUS_RUNNING);
}

void SessionTbEvent::AddResult(const Result& oResult)
{
    LOG4_DEBUG("%s", oResult.DebugString().c_str());
    if (0 == m_uiAppId)
    {
        m_uiAppId = oResult.app_id();
        m_strChannel = oResult.channel();
        m_strTag = oResult.tag();
        m_strEventId = oResult.key1();
    }
    m_iUv += oResult.uv();
    m_iPv += oResult.pv();
    m_iVv += oResult.vv();
    m_iIv += oResult.iv();
    m_llEventLength += oResult.length();
}

void SessionTbEvent::WriteResult()
{
    neb::DbOperator oDbOper(1, "tb_event", neb::Mydis::DbOperate::REPLACE, 1);
    oDbOper.AddDbField("stat_date", m_strDate);
    oDbOper.AddDbField("app_id", m_uiAppId);
    oDbOper.AddDbField("channel", m_strChannel);
    oDbOper.AddDbField("tag", m_strTag);
    oDbOper.AddDbField("event_id", m_strEventId);
    oDbOper.AddDbField("uv", m_iUv);
    oDbOper.AddDbField("pv", m_iPv);
    oDbOper.AddDbField("vv", m_iVv);
    oDbOper.AddDbField("iv", m_iIv);
    oDbOper.AddDbField("event_length", m_llEventLength);
    LOG4_DEBUG("%s", oDbOper.MakeMemOperate()->DebugString().c_str());
    auto pStep = MakeSharedStep("nebio::StepWriteDb");
    pStep->Emit(neb::ERR_OK, "", (void*)(oDbOper.MakeMemOperate()));
}

}
