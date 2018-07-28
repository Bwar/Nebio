/*******************************************************************************
 * Project:  Nebio
 * @file     CmdDataLand.cpp
 * @brief    数据采集落地
 * @author   Bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/
#include <sys/stat.h>       //need int mkdir(const char *pathname, mode_t mode)
#include <sys/types.h>      //need int mkdir(const char *pathname, mode_t mode)
#include <chrono>
#include <iomanip>
#include <sstream>
#include "CmdDataLand.hpp"

namespace nebio
{

CmdDataLand::CmdDataLand(int32 iCmd)
    : neb::Cmd(iCmd)
{
}

CmdDataLand::~CmdDataLand()
{
}

bool CmdDataLand::Init()
{
    neb::CJsonObject oEvent = GetCustomConf();
    oEvent["collect"].Get("data_path", m_strLogDataPath);
    oEvent["collect"].Get("data_file_name", m_strLogFileName);
    oEvent["collect"].Get("data_file_size", m_iLogMaxFileSize);
    if (m_strLogDataPath[0] != '/')
    {
        m_strLogDataPath = GetWorkPath() + std::string("/") + m_strLogDataPath;
    }
    if (access(m_strLogDataPath.c_str(), W_OK) != 0)
    {
        if (mkdir(m_strLogDataPath.c_str(), S_IRWXU) != 0)
        {
            LOG4_ERROR("create dir %s error!", m_strLogDataPath.c_str());
            return(false);
        }
    }
    LOG4_DEBUG("collect_data_path = %s", m_strLogDataPath.c_str());

    return(OpenDataFile());
}


bool CmdDataLand::AnyMessage(
                std::shared_ptr<neb::SocketChannel> pChannel, 
                const MsgHead& oMsgHead, const MsgBody& oMsgBody)
{
    Event oEvent;
    if (oEvent.ParseFromString(oMsgBody.data()))
    {
        return(WriteData(oEvent));
    }
    else
    {
        LOG4_ERROR("nebio::Event failed to parse MsgBody.data()!");
        return(false);
    }
}

bool CmdDataLand::WriteData(Event& oEvent)
{
    if (!OpenDataFile())
    {
        return(false);
    }
    
    m_ofs << oEvent.event_id() << "|" << oEvent.event_type() << "|" << oEvent.time() << "|"
            << oEvent.page() << "|" << oEvent.referer() << "|" << oEvent.session_id() << "|" 
            << oEvent.user_id() << "|" << oEvent.device_id() << "|" << oEvent.plat() << "|"
            << oEvent.explorer() << "|" << oEvent.client_ip() << "|" << oEvent.app_id() << std::endl;
    return(true);
}

bool CmdDataLand::OpenDataFile()
{
    if (m_ofs.is_open())
    {
        if (m_ofs.tellp() < m_iLogMaxFileSize)
        {
            return(true);
        }
        m_ofs.flush();
        m_ofs.close();
    }
    auto time_now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(time_now);
    std::ostringstream osDataFile;
    osDataFile << m_strLogDataPath << "/" << m_strLogFileName << "." << std::put_time(std::localtime(&t), "%Y%m%d");
    m_strLogDataFile = osDataFile.str();
    for (int i = 1; ; ++i)
    {
        if (access(m_strLogDataFile.c_str(), F_OK) != 0)
        {
            break;
        }
        m_strLogDataFile = osDataFile.str() + std::string(".") + std::to_string(i);
    }
    m_ofs.open(m_strLogDataFile, std::ios::app);
    if (!m_ofs.good())
    {
        LOG4_ERROR("failed to open file \"%s\"", m_strLogDataFile.c_str());
        return(false);
    }
    LOG4_TRACE("open file \"%s\" successful.", m_strLogDataFile.c_str());
    return(true);
}

} /* namespace nebio */
