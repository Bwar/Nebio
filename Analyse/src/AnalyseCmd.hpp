/*******************************************************************************
 * Project:  Nebio
 * @file     AnalyseCmd.hpp
 * @brief    统计命令字
 * @author   Bwar
 * @date:    2018年4月21日
 * @note     
 * Modify history:
 ******************************************************************************/
#ifndef ANALYSECMD_HPP_
#define ANALYSECMD_HPP_

namespace nebio
{

enum E_CMD_WORD
{
    CMD_DATA_LAND           = 1001,
    CMD_COLLECT             = 1003,
    CMD_EVENT               = 1005,
    CMD_USER                = 1007,
    CMD_PAGE                = 1009,
    CMD_TB_EVENT            = 1021,
    CMD_TB_PAGE             = 1023,
    CMD_TB_USER             = 1025,
};

}

#endif
