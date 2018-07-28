/*******************************************************************************
 * Project:  Nebio
 * @file     ModuleCollect.cpp
 * @brief    数据采集和预处理
 * @author   Bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/

#include <ctime>
#include <sstream>
#include <locale>
#include <iomanip>
#include <algorithm>
#include <util/json/CJsonObject.hpp>
#include "event.pb.h"
#include "ModuleCollect.hpp"

namespace nebio
{

ModuleCollect::ModuleCollect(const std::string& strModulePath)
    : neb::Module(strModulePath)
{
}

ModuleCollect::~ModuleCollect()
{
}

bool ModuleCollect::Init()
{
    // return(GetAppInfo());
    return(true);
}

bool ModuleCollect::AnyMessage(
        std::shared_ptr<neb::SocketChannel> pChannel,
        const HttpMsg& oHttpMsg)
{
    if (HTTP_OPTIONS == oHttpMsg.method())
    {
        LOG4_TRACE("receive an OPTIONS");
        ResponseOptions(pChannel, oHttpMsg);
        return(true);
    }
    else if (HTTP_POST == oHttpMsg.method())
    {
        neb::CJsonObject oJsonEvent;
        if (oJsonEvent.Parse(oHttpMsg.body()) && oJsonEvent.ToString().length() > 2) // there is a bug in cJSON_Parse(), we have to add "&& oJson.ToString().length() > 2" in order to avoid this bug.
        {
            MsgBody oMsgBody;
            std::string strAppKey = "";     // TODO fill with params
            uint32 uiAppId = 1;
            for (int i = 0; i < oJsonEvent.GetArraySize(); ++i)
            {
                nebio::Event oEvent;
                oEvent.set_event_id(oJsonEvent[i]("event_id"));
                oEvent.set_event_type(oJsonEvent[i]("event_type"));
                oEvent.set_page(oJsonEvent[i]("page"));
                oEvent.set_referer(oJsonEvent[i]("referer"));
                oEvent.set_session_id(oJsonEvent[i]("session_id"));
                oEvent.set_user_id(oJsonEvent[i]("user_id"));
                oEvent.set_device_id(oJsonEvent[i]("device_id"));
                oEvent.set_plat(oJsonEvent[i]("plat"));
                oEvent.set_explorer(oJsonEvent[i]("explorer"));

                std::tm tm = {};
                std::time_t event_time;
                std::istringstream ss(oJsonEvent[i]("time"));
                ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
                if (ss.fail()) 
                {
                    event_time = time(NULL);
                } 
                else 
                {
                    std::time_t event_time = std::mktime(&tm);
                }
                oEvent.set_time((uint64)event_time);

                for (int j = 0; j < oHttpMsg.params_size(); ++j)
                {
                    std::string strHeadName = oHttpMsg.params(j).name();
                    std::transform(strHeadName.begin(), strHeadName.end(), strHeadName.begin(),
                            [](unsigned char c) -> unsigned char { return std::tolower(c); });
                    if ("x-forwarded-for" == strHeadName)
                    {
                        std::string strClientIp = oHttpMsg.params(j).name().substr(0, oHttpMsg.params(j).name().find_first_of(','));
                        oEvent.set_client_ip(strClientIp);
                        break;
                    }
                }
                if (oEvent.client_ip().size() == 0)
                {
                    oEvent.set_client_ip(pChannel->GetRemoteAddr());
                }
                auto app_iter = m_mapApp.find(strAppKey);
                if (app_iter != m_mapApp.end())
                {
                    uiAppId = app_iter->second;
                }
                oEvent.set_app_id(uiAppId);

                oMsgBody.set_data(oEvent.SerializeAsString());
                oMsgBody.mutable_req_target()->set_route(oEvent.session_id());
                SendOriented("ANALYSE", 1001, GetSequence(), oMsgBody);
                SendOriented("ANALYSE", 1003, GetSequence(), oMsgBody);
            }
        }
        else
        {
            LOG4_WARNING("error json format!");
            Response(pChannel, oHttpMsg, 10002, "error json format!");
        }
    }
    else
    {
        Response(pChannel, oHttpMsg, 10001, "only accept POST!");
    }
    return(true);
}

void ModuleCollect::Response(
        std::shared_ptr<neb::SocketChannel> pChannel,
        const HttpMsg& oHttpMsg,
        int iErrno, const std::string& strErrMsg)
{
    HttpMsg oHttpOutMsg;
    neb::CJsonObject oResponseData;
    oHttpOutMsg.set_type(HTTP_RESPONSE);
    oHttpOutMsg.set_status_code(200);
    oHttpOutMsg.set_http_major(oHttpMsg.http_major());
    oHttpOutMsg.set_http_minor(oHttpMsg.http_minor());
    oResponseData.Add("code", iErrno);
    oResponseData.Add("msg", strErrMsg);
    oHttpOutMsg.set_body(oResponseData.ToFormattedString());
    SendTo(pChannel, oHttpMsg);
}

void ModuleCollect::ResponseOptions(
        std::shared_ptr<neb::SocketChannel> pChannel,
        const HttpMsg& oHttpMsg)
{
    LOG4_TRACE(" ");
    HttpMsg oHttpOutMsg;
    oHttpOutMsg.set_type(HTTP_RESPONSE);
    oHttpOutMsg.set_status_code(200);
    oHttpOutMsg.set_http_major(oHttpMsg.http_major());
    oHttpOutMsg.set_http_minor(oHttpMsg.http_minor());
    HttpMsg::Header* pHeader = oHttpOutMsg.add_headers();
    pHeader->set_header_name("Access-Control-Allow-Origin");
    pHeader->set_header_value("*");
    pHeader = oHttpOutMsg.add_headers();
    pHeader->set_header_name("Access-Control-Allow-Headers");
    pHeader->set_header_value("Origin, Content-Type, Cookie, Accept, multipart/form-data, application/json, token,x-token,Access-Token,X-CSRF-TOKEN, Accept, Authorization, X-XSRF-TOKEN");
    pHeader = oHttpOutMsg.add_headers();
    pHeader->set_header_name("Access-Control-Allow-Methods");
    pHeader->set_header_value("GET, POST");
    pHeader = oHttpOutMsg.add_headers();
    pHeader->set_header_name("Access-Control-Allow-Credentials");
    pHeader->set_header_value("true");
    SendTo(pChannel, oHttpMsg);
}

}   // namespace nebio

