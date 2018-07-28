/*******************************************************************************
 * Project:  Nebio
 * @file     ModuleCollect.hpp
 * @brief    数据采集和预处理
 * @author   Bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/
#ifndef SRC_MODULECOLLECT_HPP_
#define SRC_MODULECOLLECT_HPP_

#include <string>
#include <map>
#include <Error.hpp>
#include <actor/cmd/Module.hpp>

namespace nebio 
{

class ModuleCollect: public neb::Module, public neb::DynamicCreator<ModuleCollect, std::string>
{
public:
    ModuleCollect(const std::string& strModulePath);
    virtual ~ModuleCollect();

    virtual bool Init();

    virtual bool AnyMessage(
                    std::shared_ptr<neb::SocketChannel> pChannel,
                    const HttpMsg& oHttpMsg);
protected:
    void Response(
            std::shared_ptr<neb::SocketChannel> pChannel,
            const HttpMsg& oHttpMsg, int iErrno, const std::string& strErrMsg);
    void ResponseOptions(
            std::shared_ptr<neb::SocketChannel> pChannel,
            const HttpMsg& oHttpMsg);

private:
    std::unordered_map<std::string, uint32> m_mapApp;       // appkey : appid
};

} /* namespace nebio */

#endif /* SRC_MODULESWITCH_MODULESWITCH_HPP_ */
