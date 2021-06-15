/**
 *  聊天服务程序入口函数
 **/
#include <iostream>
#include <stdlib.h>

#include "../base/Platform.h"
#include "../base/Singleton.h"
#include "../base/ConfigFileReader.h"
#include "../base/AsyncLog.h"
#include "../net/EventLoop.h"
#include "../net/EventLoopThreadPool.h"
#include "../mysqlmgr/MysqlManager.h"

#include <string.h>
#include "../utils/DaemonRun.h"

#include "UserManager.h"
#include "ChatServer.h"
#include "HttpServer.h"

using namespace std;

EventLoop g_mainLoop;

void prog_exit(int signo) {
    cout<<"program recv signal ["<<signo<<"] to exit."<<endl;
    Singleton<HttpServer>::Instance().uninit();
    Singleton<ChatServer>::Instance().uninit();
    g_mainLoop.quit();

    CAsyncLog::uninit();
}

int main(int argc,char* argv[]) {
    //设置信号处理函数
    signal(SIGCHLD, SIG_DFL);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, prog_exit);
    signal(SIGTERM, prog_exit);

    int ch;
    bool bdaemon = false;
    while((ch = getopt(argc,argv,"d")) != -1) {
        switch (ch)
        {
        case 'd':
            bdaemon = true;
            break;
        }
    }
    
    if(bdaemon) daemon_run();

    CConfigFileReader config("etc/chatserver.conf");

    const char* logbinarypackage = config.getConfigName("logbinarypackage");
    if(logbinarypackage != NULL) {
        int logbinarypackageint = atoi(logbinarypackage);
        if(logbinarypackage != 0) {
            Singleton<ChatServer>::Instance().enableLogPackageBinary(true);
        }else {
            Singleton<ChatServer>::Instance().enableLogPackageBinary(false);
        }
    }

    string logFileFullPath;
    const char* logFilePath = config.getConfigName("logfiledir");
    if(logFilePath == NULL) {
        LOGF("logdir is not set in config file");
        return 1;
    }

    //log目录不存在，创建之
    DIR* dp = opendir(logFilePath);
    if(dp == NULL) {
        if (mkdir(logFilePath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
        {
            LOGF("create base dir error, %s , errno: %d, %s", logFilePath, errno, strerror(errno));
            return 1;
        }
    }

    closedir(dp);

    logFileFullPath = logFilePath;

    const char* logfilename = config.getConfigName("logfilename");
    logFileFullPath += logfilename;

    CAsyncLog::init();

    //初始化数据库配置
    const char* dbserver = config.getConfigName("dbserver");
    const char* dbuser = config.getConfigName("dbuser");
    const char* dbpassword = config.getConfigName("dbpassword");
    const char* dbname = config.getConfigName("dbname");
    if(!Singleton<CMysqlManager>::Instance().init(dbserver,dbuser,dbpassword,dbname)) {
        LOGF("Init mysql failed, please check your database config......");
    }

    if(!Singleton<UserManager>::Instance().init(dbserver,dbuser,dbpassword,dbname)) {
        LOGF("Init User Manager failed, please check your database config......");
    }

    const char* listenip = config.getConfigName("listenip");
    short listenport = static_cast<short>(atoi(config.getConfigName("listenport")));
    Singleton<ChatServer>::Instance().init(listenip,listenport,&g_mainLoop);

    const char* httplistenip = config.getConfigName("httplistenip");
    short httplistenport = static_cast<short>(atol(config.getConfigName("httplistenport")));
    Singleton<HttpServer>::Instance().init(httplistenip, httplistenport, &g_mainLoop);

    LOGI("chatserver initialization completed , now you can use client to connect it");

    g_mainLoop.loop();

    LOGI("exit chatserver");

    return 0;
}