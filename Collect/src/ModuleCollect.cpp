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
#include <util/http/http_parser.h>
#include <util/StringCoder.hpp>
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
            std::string strAppKey = "";     // TODO fill with params
            uint32 uiAppId = 1;
            std::string strClientIp = pChannel->GetRemoteAddr();

            auto app_iter = m_mapApp.find(strAppKey);
            if (app_iter != m_mapApp.end())
            {
                uiAppId = app_iter->second;
            }
            for (auto it = oHttpMsg.headers().begin(); it != oHttpMsg.headers().end(); ++it)
            {
                std::string strHeadName = it->first;
                std::transform(strHeadName.begin(), strHeadName.end(), strHeadName.begin(),
                        [](unsigned char c) -> unsigned char { return std::tolower(c); });
                if ("x-forwarded-for" == strHeadName)
                {
                    strClientIp = it->second.substr(0, it->second.find_first_of(','));
                    break;
                }
            }

            if (oJsonEvent.GetArraySize() == 0)
            {
                TransferEvent(uiAppId, strClientIp, oJsonEvent);
            }
            for (int i = 0; i < oJsonEvent.GetArraySize(); ++i)
            {
                TransferEvent(uiAppId, strClientIp, oJsonEvent[i]);
            }

            Response(pChannel, oHttpMsg, neb::ERR_OK, "success");
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

void ModuleCollect::TransferEvent(uint32 uiAppId, const std::string& strClientIp, const neb::CJsonObject& oJsonEvent)
{
    MsgBody oMsgBody;
    nebio::Event oEvent;
    oEvent.set_event_id(oJsonEvent("event_id"));
    oEvent.set_event_type(oJsonEvent("event_type"));
    oEvent.set_page(oJsonEvent("page"));
    oEvent.set_referer(oJsonEvent("referer"));
    oEvent.set_session_id(oJsonEvent("session_id"));
    oEvent.set_user_id(oJsonEvent("user_id"));
    oEvent.set_device_id(oJsonEvent("device_id"));
    oEvent.set_plat(oJsonEvent("plat"));
    oEvent.set_explorer(oJsonEvent("explorer"));

    std::tm tm = {};
    std::time_t event_time;
    std::istringstream ss(oJsonEvent("time"));
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    if (ss.fail()) 
    {
        event_time = time(NULL);
    } 
    else 
    {
        event_time = std::mktime(&tm);
    }
    oEvent.set_time((uint64)event_time);

    oEvent.set_client_ip(strClientIp);
    oEvent.set_app_id(uiAppId);

    oMsgBody.set_data(oEvent.SerializeAsString());
    oMsgBody.mutable_req_target()->set_route(oEvent.session_id());
    SendOriented("ANALYSE", 1001, GetSequence(), oMsgBody);
    SendOriented("ANALYSE", 1003, GetSequence(), oMsgBody);
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
    oHttpOutMsg.mutable_headers()->insert(google::protobuf::MapPair<std::string, std::string>("Connection", "keep-alive"));
    oHttpOutMsg.mutable_headers()->insert(google::protobuf::MapPair<std::string, std::string>("Access-Control-Allow-Origin", "*"));
    oHttpOutMsg.mutable_headers()->insert(google::protobuf::MapPair<std::string, std::string>("Access-Control-Allow-Headers", "Origin, Content-Type, Cookie, Accept"));
    oHttpOutMsg.mutable_headers()->insert(google::protobuf::MapPair<std::string, std::string>("Access-Control-Allow-Methods", "GET, POST"));
    oHttpOutMsg.mutable_headers()->insert(google::protobuf::MapPair<std::string, std::string>("Access-Control-Allow-Credentials", "true"));
    oHttpOutMsg.set_body(oResponseData.ToFormattedString());
    SendTo(pChannel, oHttpOutMsg);
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
    oHttpOutMsg.mutable_headers()->insert(google::protobuf::MapPair<std::string, std::string>("Connection", "keep-alive"));
    oHttpOutMsg.mutable_headers()->insert(google::protobuf::MapPair<std::string, std::string>("Access-Control-Allow-Origin", "*"));
    oHttpOutMsg.mutable_headers()->insert(google::protobuf::MapPair<std::string, std::string>("Access-Control-Allow-Headers", "Origin, Content-Type, Cookie, Accept"));
    oHttpOutMsg.mutable_headers()->insert(google::protobuf::MapPair<std::string, std::string>("Access-Control-Allow-Methods", "GET, POST"));
    oHttpOutMsg.mutable_headers()->insert(google::protobuf::MapPair<std::string, std::string>("Access-Control-Allow-Credentials", "true"));
    SendTo(pChannel, oHttpOutMsg);
}

}   // namespace nebio

