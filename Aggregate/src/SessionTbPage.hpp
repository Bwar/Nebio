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
    public neb::DynamicCreator<SessionTbPage, std::string, uint32, std::string, ev_tstamp>
{
public:
    SessionTbPage(const std::string& strSessionId,
            uint32 uiDate, const std::string& strDate, ev_tstamp dSessionTimeout = 10.0);
    virtual ~SessionTbPage();

    virtual neb::E_CMD_STATUS Timeout();

    void AddResult(const Result& oResult);

protected:
    void WriteResult();

private:
    uint32 m_uiDate;        // YYYYMMDD
    std::string m_strDate;  // YYYY-MM-DD
    uint32 m_uiAppId;
    std::string m_strChannel;
    std::string m_strTag;
    std::string m_strPage;
     
    int32 m_iUv;
    int32 m_iPv;
    int32 m_iVv;
    int32 m_iIv;
    int32 m_iExitVv;
    int32 m_iBounceVv;
    int64 m_llOnlineTime;
};

} // namespace nebio

#endif // SESSIONPAGE_HPP_

