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
    public neb::DynamicCreator<SessionTbUser, std::string, uint32, std::string, ev_tstamp>
{
public:
    SessionTbUser(const std::string& strSessionId,
            uint32 uiDate, const std::string& strDate, ev_tstamp dSessionTimeout = 10.0);
    virtual ~SessionTbUser();

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
    uint32 m_uiUserType;
     
    int32 m_iUv;
    int32 m_iPv;
    int32 m_iVv;
    int32 m_iIv;
    int64 m_llSessionLength;
};

} // namespace nebio

#endif // SESSIONTBUSER_HPP_

