/*******************************************************************************
 * Project:  Nebio
 * @file     SessionUserIv.hpp
 * @brief    事件统计
 * @author   bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/
#ifndef SESSIONUSERIV_HPP_
#define SESSIONUSERIV_HPP_

#include <string>
#include <unordered_set>
#include <unordered_map>
#include "AnalyseCmd.hpp"
#include "AnalyseTimer.hpp"

namespace nebio
{

class SessionUserIv : public AnalyseTimer,
    public neb::DynamicCreator<SessionUserIv, std::string, std::string, std::string, uint32, ev_tstamp>
{
public:
    SessionUserIv(const std::string& strSessionId, const std::string& strChannel, const std::string& strTag,
        uint32 uiDate, ev_tstamp dSessionTimeout = 10.0);
    virtual ~SessionUserIv();

    virtual neb::E_CMD_STATUS Timeout();

    void AddEvent(const Event& oEvent);

protected:
    void AddUser(const Event& oEvent);
    void AddTourist(const Event& oEvent);
    void DelTourist(const Event& oEvent);
    void FlushOut();

private:
    uint32 m_uiDate;        // YYYYMMDD
    uint32 m_uiAppId;
    std::string m_strChannel;
    std::string m_strTag;

    int32 m_iActivityUserIv;
    int32 m_iNewUserIv;
    int32 m_iHistoryUserIv;
    int32 m_iTouristIv;
    std::unordered_set<std::string> m_setIp;
};

} // namespace nebio

#endif // SESSIONUSERIV_HPP_

