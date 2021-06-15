/**
 *  服务器主服务类，IMServer.cpp
 **/
#include "ChatServer.h"

#include "../net/InetAddress.h"
#include "../base/AsyncLog.h"
#include "../base/Singleton.h"
#include "ChatSession.h"
#include "UserManager.h"

ChatServer::ChatServer() {
    m_logPackageBinary = false;
}

bool ChatServer::init(const char* ip,short port,EventLoop* loop) {
    InetAddress addr(ip,port);

    m_server.reset(new TcpServer(loop,addr,"EASYIM-SERVER",TcpServer::kReusePort));
    m_server->setConnectionCallback(std::bind(&ChatServer::onConnected,this,std::placeholders::_1));
    m_server->start(6);

    return true;
}

void ChatServer::uninit() {
    if(m_server) m_server->stop();
}

void ChatServer::enableLogPackageBinary(bool enable) {
    m_logPackageBinary = enable;
}

bool ChatServer::isLogPackageBinaryEnabled() {
    return m_logPackageBinary;
}

void ChatServer::onConnected(std::shared_ptr<TcpConnection> conn) {
    if(conn->connected()) {
        LOGD("client connected: %s",conn->peerAddress().toIpPort().c_str());
        ++m_sessionId;
        std::shared_ptr<ChatSession> spSession(new ChatSession(conn,m_sessionId));
        conn->setMessageCallback(std::bind(&ChatSession::onRead,spSession.get(),std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));

        std::unique_lock<std::mutex> guard(m_sessionMutex);
        m_sessions.emplace_back(std::move(spSession));
    }else {
        onDisconnected(conn);
    }
}

void ChatServer::onDisconnected(const std::shared_ptr<TcpConnection>& conn) {
    UserManager& userMgr = Singleton<UserManager>::Instance();

    std::unique_lock<std::mutex> guard(m_sessionMutex);
    for(auto iter = m_sessions.begin(); iter != m_sessions.end(); ++iter) {
        if((*iter)->getConnectionPtr() == NULL) {
            LOGE("connection is NULL");
            break;
        }
        //比对Connection对象找到对应的session
        if((*iter)->getConnectionPtr() == conn) {
            //该Session不是之前被踢下线的有效session才认为是正常下线，给其好友推送其下线消息
            if((*iter)->isSessionValid()) {
                //遍历其在线好友，给其好友推送下线消息
                std::list<User> friends;
                int32_t offlineUserId = (*iter)->getUserId();
                userMgr.getFriendInfoByUserId(offlineUserId,friends);
                for(const auto& iter_inner : friends) {
                    for(auto& ss : m_sessions) {
                        //该好友是否在线
                        if(iter_inner.userid == ss->getUserId()) {
                            ss->sendUserStatusChangeMsg(offlineUserId,2);

                            LOGI("SendUserStatusChangeMsg to user(userid=%d): user go offline, offline userid = %d", ss->getUserId(), offlineUserId);
                        }
                    }
                }
            }else {
                LOGI("Session is invalid, userid=%d",(*iter)->getUserId());
            }

            m_sessions.erase(iter);
            LOGI("client disconnected: %s",conn->peerAddress().toIpPort().c_str());
            break;
        }
    }

    LOGI("current online user count: %d",static_cast<int>(m_sessions.size()));
}

void ChatServer::getSessions(std::list<std::shared_ptr<ChatSession>>& sessions) {
    std::unique_lock<std::mutex> guard(m_sessionMutex);
    sessions = m_sessions;
}

bool ChatServer::getSessionByUserIdAndClientType(std::shared_ptr<ChatSession>& session,int32_t userid,int32_t clientType) {
    std::unique_lock<std::mutex> lck(m_sessionMutex);
    std::shared_ptr<ChatSession> guard;
    for(const auto& iter : m_sessions) {
        guard = iter;
        if(iter->getUserId() == userid && iter->getClientType() == clientType) {
            session = guard;
            return true;
        }
    }
    return false;
}

bool ChatServer::getSessionsByUserId(std::list<std::shared_ptr<ChatSession>>& sessions,int32_t userid) {
    std::unique_lock<std::mutex> lck(m_sessionMutex);
    std::shared_ptr<ChatSession> guard;
    for(const auto& iter : m_sessions) {
        guard = iter;
        if(iter->getUserId() == userid) {
            sessions.emplace_back(guard);
            return true;
        }
    }

    return false;
}

int32_t ChatServer::getUserStatusByUserId(int32_t userid) {
    std::unique_lock<std::mutex> guard(m_sessionMutex);
    for(const auto& iter : m_sessions) {
       if(iter->getUserId() == userid) {
           return iter->getUserStatus();
       } 
    }

    return 0;
}

int32_t ChatServer::getUserClientTypeByUserId(int32_t userid) {
    std::unique_lock<std::mutex> guard(m_sessionMutex);
    bool bMobileOnline = false;
    int clientType = CLIENT_TYPE_UNKOWN;
    for(const auto& iter : m_sessions) {
        if(iter->getUserId() == userid) {
            clientType = iter->getClientType();
            if(clientType == CLIENT_TYPE_PC) {
                return clientType;
            }
            else if(clientType == CLIENT_TYPE_ANDROID) {
                bMobileOnline = true;
            }
        }
    }

    if(bMobileOnline) return clientType;

    return CLIENT_TYPE_UNKOWN;
}

