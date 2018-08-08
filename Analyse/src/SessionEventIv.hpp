/*******************************************************************************
 * Project:  Nebio
 * @file     SessionEventIv.hpp
 * @brief    事件IP数统计
 * @author   bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/
#ifndef SESSIONEVENTIV_HPP_
#define SESSIONEVENTIV_HPP_

#include <string>
#include <unordered_set>
#include <unordered_map>
#include "AnalyseSession.hpp"

namespace nebio
{

class SessionEventIv : public AnalyseSession,
    public neb::DynamicCreator<SessionEventIv, std::string, std::string, std::string, ev_tstamp>
{
public:
    SessionEventIv(const std::string& strSessionId, const std::string& strChannel, const std::string& strTag,
        ev_tstamp dSessionTimeout = 10.0);
    virtual ~SessionEventIv();

    virtual neb::E_CMD_STATUS Timeout();

    void AddEvent(const Event& oEvent);

protected:
    void FlushOut();

private:
    uint32 m_uiAppId;
    std::string m_strChannel;
    std::string m_strTag;
    std::string m_strEventId;
    std::string m_strEventType;
    std::string m_strPage;   
     
    int32 m_iIv;
    std::unordered_set<std::string> m_setIp;
};

} // namespace nebio

#endif // SESSIONEVNETIV_HPP_

