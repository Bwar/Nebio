/*******************************************************************************
 * Project:  Nebio
 * @file     SessionTbPage.hpp
 * @brief    页面统计结果
 * @author   bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/
#ifndef SESSIONPAGE_HPP_
#define SESSIONPAGE_HPP_

#include <string>
#include <unordered_set>
#include <unordered_map>
#include "AggregateTimer.hpp"

namespace nebio
{

class SessionTbPage : public AggregateTimer,
    public neb::DynamicCreator<SessionTbPage, std::string, ev_tstamp>
{
public:
    SessionTbPage(const std::string& strSessionId, ev_tstamp dSessionTimeout = 10.0);
    virtual ~SessionTbPage();

    virtual neb::E_CMD_STATUS Timeout();

    void AddResult(const Result& oResult);

protected:
    void WriteResult();

private:
    uint32 m_uiAppId;
    std::string m_strChannel;
    std::string m_strTag;
    std::string m_strPage;
     
    uint32 m_uiUv;
    uint32 m_uiPv;
    uint32 m_uiVv;
    uint32 m_uiIv;
    uint32 m_uiExitVv;
    uint32 m_uiBounceVv;
    uint64 m_ullOnlineTime;
};

} // namespace nebio

#endif // SESSIONPAGE_HPP_

