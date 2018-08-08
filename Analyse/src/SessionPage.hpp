/*******************************************************************************
 * Project:  Nebio
 * @file     SessionPage.hpp
 * @brief    页面统计
 * @author   bwar
 * @date:    2018年4月22日
 * @note     
 * Modify history:
 ******************************************************************************/
#ifndef SESSIONPAGE_HPP_
#define SESSIONPAGE_HPP_

#include <string>
#include <unordered_set>
#include <unordered_map>
#include "AnalyseSession.hpp"

namespace nebio
{

class SessionPage : public AnalyseSession,
    public neb::DynamicCreator<SessionPage, std::string, std::string, std::string, ev_tstamp>
{
public:
    SessionPage(const std::string& strSessionId, const std::string& strChannel, const std::string& strTag,
        ev_tstamp dSessionTimeout = 10.0);
    virtual ~SessionPage();

    virtual neb::E_CMD_STATUS Timeout();

    void AddEvent(const Event& oEvent);

protected:
    void FlushOut();

private:
    uint32 m_uiAppId;
    std::string m_strChannel;
    std::string m_strTag;
    std::string m_strPage;   
     
    int32 m_iPv;
    int32 m_iUv;
    int32 m_iVv;
    int32 m_iExitVv;
    int32 m_iBounceVv;
    int64 m_llPageLength;
    std::unordered_set<std::string> m_setUserId;
};

} // namespace nebio

#endif // SESSIONEVNET_HPP_

