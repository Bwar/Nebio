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
#include <util/StringCoder.hpp>
#include "UnixTime.hpp"
#include "CmdDataLand.hpp"

namespace nebio
{

CmdDataLand::CmdDataLand(int32 iCmd)
    : neb::Cmd(iCmd),
      m_iFileDate(0), m_iFileIndex(0), m_iLogMaxFileSize(65535)
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
    m_iFileDate = atoi(neb::time_t2TimeStr((time_t)GetNowTime(), "YYYYMMDD").c_str());

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
    
    std::string strPage = oEvent.page();
    std::string strReferer = oEvent.referer();
    /*
    if (strPage.find_first_of('?')  != std::string::npos)
    {
        std::string strParam = std::move(strPage.substr(strPage.find_first_of('?') + 1, std::string::npos));
        std::string strUrl = std::move(strPage.substr(0, strPage.find_first_of('?')));
        strPage = strUrl + std::string("?") + neb::UrlEncode(strParam);
    }
    if (strReferer.find_first_of('?') != std::string::npos)
    {
        std::string strParam = std::move(strReferer.substr(strReferer.find_first_of('?') + 1, std::string::npos));
        std::string strUrl = std::move(strReferer.substr(0, strReferer.find_first_of('?')));
        strReferer = strUrl + std::string("?") + neb::UrlEncode(strParam);
    }
    */
    m_ofs << oEvent.event_id() << "|" << oEvent.event_type() << "|" << oEvent.time() << "|"
            << strPage << "|" << strReferer << "|" << oEvent.session_id() << "|" 
            << oEvent.user_id() << "|" << oEvent.device_id() << "|" << oEvent.plat() << "|"
            << oEvent.explorer() << "|" << oEvent.client_ip() << "|" << oEvent.app_id() << std::endl;
    return(true);
}

bool CmdDataLand::OpenDataFile()
{
    if (m_ofs.is_open())
    {
        int32 iFileDate = atoi(neb::time_t2TimeStr((time_t)GetNowTime(), "YYYYMMDD").c_str());
        if (m_ofs.tellp() < m_iLogMaxFileSize && iFileDate <= m_iFileDate)
        {
            return(true);
        }
        m_ofs.flush();
        m_ofs.close();
        if (iFileDate > m_iFileDate)
        {
            m_iFileDate = iFileDate;
            m_iFileIndex = 0;
        }
        else if (m_ofs.tellp() < m_iLogMaxFileSize)
        {
            ++m_iFileIndex;
        }
    }
    auto time_now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(time_now);
    std::ostringstream osDataFile;
    if (m_iFileIndex == 0)
    {
        osDataFile << m_strLogDataPath << "/" << m_strLogFileName << "." << std::put_time(std::localtime(&t), "%Y%m%d");
    }
    else
    {
        osDataFile << m_strLogDataPath << "/" << m_strLogFileName << "." << std::put_time(std::localtime(&t), "%Y%m%d")
                   << "." << std::to_string(m_iFileIndex);
    }
    m_strLogDataFile = osDataFile.str();
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
