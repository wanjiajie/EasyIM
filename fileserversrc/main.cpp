/**
 *  文件服务程序入口函数
 **/
#include <iostream>
#include <stdlib.h>

#include "../base/Platform.h"
#include "../base/Singleton.h"
#include "../base/ConfigFileReader.h"
#include "../base/AsyncLog.h"
#include "../net/EventLoop.h"
#include "FileManager.h"

#include <string.h>
#include "../utils/DaemonRun.h"

#include "FileServer.h"

using namespace net;

EventLoop g_mainLoop;

#ifndef WIN32
void prog_exit(int signo)
{
    std::cout << "program recv signal [" << signo << "] to exit." << std::endl;

    Singleton<FileServer>::Instance().shutdown();
    g_mainLoop.quit();
}
#endif

int main(int argc, char* argv[])
{
    //设置信号处理
    signal(SIGCHLD, SIG_DFL);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, prog_exit);
    signal(SIGTERM, prog_exit);


    int ch;
    bool bdaemon = false;
    while ((ch = getopt(argc, argv, "d")) != -1)
    {
        switch (ch)
        {
        case 'd':
            bdaemon = true;
            break;
        }
    }

    if (bdaemon)
        daemon_run();

    CConfigFileReader config("etc/fileserver.conf");

    std::string logFileFullPath;

    const char* logfilepath = config.getConfigName("logfiledir");
    if (logfilepath == NULL)
    {
        LOGF("logdir is not set in config file");
        return 1;
    }

    //如果log目录不存在则创建之
    DIR* dp = opendir(logfilepath);
    if (dp == NULL)
    {
        if (mkdir(logfilepath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
        {
            LOGF("create base dir error, %s , errno: %d, %s", logfilepath, errno, strerror(errno));
            return 1;
        }
}
    closedir(dp);

    logFileFullPath = logfilepath;

    const char* logfilename = config.getConfigName("logfilename");
    logFileFullPath += logfilename;

    CAsyncLog::init(logFileFullPath.c_str());

    const char* filecachedir = config.getConfigName("filecachedir");
    Singleton<FileManager>::Instance().init(filecachedir);   

    const char* listenip = config.getConfigName("listenip");
    short listenport = (short)atol(config.getConfigName("listenport"));
    Singleton<FileServer>::Instance().init(listenip, listenport, &g_mainLoop, filecachedir);

    LOGI("fileserver initialization completed, now you can use client to connect it.");
    
    g_mainLoop.loop();

    LOGI("exit fileserver.");

    return 0;
}
