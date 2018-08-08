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

SessionTbEvent::SessionTbEvent(const std::string& strSessionId, ev_tstamp dSessionTimeout)
    : AggregateSession(strSessionId, dSessionTimeout),
      m_uiAppId(0), m_uiUv(0), m_uiPv(0), m_uiVv(0), m_uiIv(0), m_ullEventLength(0)
{
}

SessionTbEvent::~SessionTbEvent()
{
}

neb::E_CMD_STATUS SessionTbEvent::Timeout()
{
    WriteResult();
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
    m_uiUv += oResult.uv();
    m_uiPv += oResult.pv();
    m_uiVv += oResult.vv();
    m_uiIv += oResult.iv();
    m_ullEventLength += oResult.length();
}

void SessionTbEvent::WriteResult()
{
    neb::DbOperator oDbOper(1, "tb_event", neb::Mydis::DbOperate::REPLACE, 1);
    oDbOper.AddDbField("stat_date", neb::time_t2TimeStr((time_t)GetNowTime(), "%Y-%m-%d"));
    oDbOper.AddDbField("app_id", m_uiAppId);
    oDbOper.AddDbField("channel", m_strChannel);
    oDbOper.AddDbField("tag", m_strTag);
    oDbOper.AddDbField("event_id", m_strEventId);
    oDbOper.AddDbField("uv", m_uiUv);
    oDbOper.AddDbField("pv", m_uiPv);
    oDbOper.AddDbField("vv", m_uiVv);
    oDbOper.AddDbField("iv", m_uiIv);
    oDbOper.AddDbField("event_length", m_ullEventLength);
    LOG4_DEBUG("%s", oDbOper.MakeMemOperate()->DebugString().c_str());
    auto pStep = MakeSharedStep("nebio::StepWriteDb");
    pStep->Emit(neb::ERR_OK, "", (void*)(oDbOper.MakeMemOperate()));
}

}
