#include "FileServer.h"

#include "../net/InetAddress.h"
#include "../base/AsyncLog.h"
#include "../base/Singleton.h"
#include "FileSession.h"


bool FileServer::init(const char* ip, short port, EventLoop* loop, const char* fileBaseDir/* = "filecache/"*/)
{
    m_fileBaseDir = fileBaseDir;

    InetAddress addr(ip, port);
    m_server.reset(new TcpServer(loop, addr, "Img/FileServer", TcpServer::kReusePort));
    m_server->setConnectionCallback(std::bind(&FileServer::onConnected, this, std::placeholders::_1));
    //启动侦听
    m_server->start(6);

    return true;
}

void FileServer::shutdown() {
    if(m_server) m_server->stop();
}

void FileServer::onConnected(std::shared_ptr<TcpConnection> conn) {
    if(conn->connected()) {
        LOGI("client connected: %s",conn->peerAddress().toIpPort().c_str());
        std::shared_ptr<FileSession> spSession(new FileSession(conn,m_fileBaseDir.c_str()));
        conn->setMessageCallback(std::bind(&FileSession::onRead,spSession.get(),std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));
        
        std::unique_lock<std::mutex> guard(m_mtx_session);
        m_sessions.emplace_back(std::move(spSession));
    }else {
        onDisconnected(conn);
    }
}

void FileServer::onDisconnected(const std::shared_ptr<TcpConnection>& conn) {
    std::unique_lock<std::mutex> guard(m_mtx_session);
    for(auto iter = m_sessions.begin(); iter != m_sessions.end() ; ++iter) {
        if((*iter)->getConnectionPtr() == NULL) {
            LOGE("connection is NULL");
            break;
        }

        if((*iter)->getConnectionPtr() == conn) {
            m_sessions.erase(iter);
            LOGI("client disconnected: %s", conn->peerAddress().toIpPort().c_str());
            break;       
        }
    }
}