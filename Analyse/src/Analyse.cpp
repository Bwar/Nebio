/*******************************************************************************
* Project:  NebulaLogic
* @file     Logic.cpp
* @brief 
* @author   bwar
* @date:    Apr 7, 2018
* @note
* Modify history:
******************************************************************************/

#include "util/proctitle_helper.h"
#include "labor/Manager.hpp"

int main(int argc, char* argv[])
{
    signal(SIGPIPE, SIG_IGN);
    ngx_init_setproctitle(argc, argv);
    if (argc == 2)
    {
        neb::Manager oManager(argv[1]);
        oManager.Run();
    }
    else
    {
        std::cerr << "param error! usage: " << argv[0] << "${config_file}" << std::endl;
        exit(-1);
    }
    return(0);
}

