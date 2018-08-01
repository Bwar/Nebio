/*******************************************************************************
 * Project:  Nebio
 * @file     SessionTbUser.cpp
 * @brief    用户统计
 * @author   bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/

#include <mydis/DbOperator.hpp>
#include "UnixTime.hpp"
#include "SessionTbUser.hpp"
#include "StepWriteDb.hpp"

namespace nebio
{

SessionTbUser::SessionTbUser(const std::string& strSessionId, ev_tstamp dSessionTimeout)
    : AggregateSession(strSessionId, dSessionTimeout),
      m_uiUserType(0), m_uiUv(0), m_uiPv(0), m_uiVv(0), m_uiIv(0), m_ullSessionLength(0)
{
}

SessionTbUser::~SessionTbUser()
{
}

neb::E_CMD_STATUS SessionTbUser::Timeout()
{
    FlushOut();
    return(neb::CMD_STATUS_RUNNING);
}

void SessionTbUser::AddResult(const Result& oResult)
{
    if (0 == m_uiAppId)
    {
        m_uiAppId = oResult.app_id();
        m_strChannel = oResult.channel();
        m_strTag = oResult.tag();
        m_uiUserType = oResult.key5();
    }
    m_uiUv += oResult.uv();
    m_uiPv += oResult.pv();
    m_uiVv += oResult.vv();
    m_uiIv += oResult.iv();
    m_ullSessionLength += oResult.length();
}

void SessionTbUser::FlushOut()
{
    neb::DbOperator oDbOper(0, "tb_user", neb::Mydis::DbOperate::REPLACE);
    oDbOper.AddDbField("stat_date", neb::time_t2TimeStr((time_t)GetNowTime(), "%Y-%m-%d"));
    oDbOper.AddDbField("app_id", m_uiAppId);
    oDbOper.AddDbField("channel", m_strChannel);
    oDbOper.AddDbField("tag", m_strTag);
    oDbOper.AddDbField("user_type", m_uiUserType);
    oDbOper.AddDbField("uv", m_uiUv);
    oDbOper.AddDbField("pv", m_uiPv);
    oDbOper.AddDbField("vv", m_uiVv);
    oDbOper.AddDbField("iv", m_uiIv);
    oDbOper.AddDbField("session_length", m_ullSessionLength);
    auto pStep = MakeSharedStep("nebio::StepWriteDb");
    pStep->Emit(neb::ERR_OK, "", (void*)(oDbOper.MakeMemOperate()));
    m_uiUv = 0;
    m_uiPv = 0;
    m_uiVv = 0;
    m_uiIv = 0;
    m_ullSessionLength = 0;
}

}
