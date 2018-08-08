/*******************************************************************************
 * Project:  Nebio
 * @file     SessionPageIv.hpp
 * @brief    页面统计
 * @author   bwar
 * @date:    2018年4月22日
 * @note     
 * Modify history:
 ******************************************************************************/
#ifndef SESSIONPAGEIV_HPP_
#define SESSIONPAGEIV_HPP_

#include <string>
#include <unordered_set>
#include <unordered_map>
#include "AnalyseSession.hpp"

namespace nebio
{

class SessionPageIv : public AnalyseSession,
    public neb::DynamicCreator<SessionPageIv, std::string, std::string, std::string, ev_tstamp>
{
public:
    SessionPageIv(const std::string& strSessionId, const std::string& strChannel, const std::string& strTag,
        ev_tstamp dSessionTimeout = 10.0);
    virtual ~SessionPageIv();

    virtual neb::E_CMD_STATUS Timeout();

    void AddEvent(const Event& oEvent);

protected:
    void FlushOut();

private:
    uint32 m_uiAppId;
    std::string m_strChannel;
    std::string m_strTag;
    std::string m_strPage;   
     
    int32 m_iIv;
    std::unordered_set<std::string> m_setIp;
};

} // namespace nebio

#endif // SESSIONEVNETIV_HPP_

