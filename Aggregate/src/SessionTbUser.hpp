/*******************************************************************************
 * Project:  Nebio
 * @file     SessionTbUser.hpp
 * @brief    用户统计
 * @author   bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/
#ifndef SESSIONEVENT_HPP_
#define SESSIONEVENT_HPP_

#include <string>
#include <unordered_set>
#include <unordered_map>
#include "AggregateTimer.hpp"

namespace nebio
{

class SessionTbUser : public AggregateTimer,
    public neb::DynamicCreator<SessionTbUser, std::string, ev_tstamp>
{
public:
    SessionTbUser(const std::string& strSessionId, ev_tstamp dSessionTimeout = 10.0);
    virtual ~SessionTbUser();

    virtual neb::E_CMD_STATUS Timeout();

    void AddResult(const Result& oResult);

protected:
    void WriteResult();

private:
    uint32 m_uiAppId;
    std::string m_strChannel;
    std::string m_strTag;
    uint32 m_uiUserType;
     
    uint32 m_uiUv;
    uint32 m_uiPv;
    uint32 m_uiVv;
    uint32 m_uiIv;
    uint64 m_ullSessionLength;
};

} // namespace nebio

#endif // SESSIONTBUSER_HPP_

