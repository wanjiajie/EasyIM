/** 
 *  服务器主服务类，IMServer.h
 **/
#pragma once

#include <memory>
#include <list>
#include <map>
#include <mutex>
#include <atomic>
#include "../net/TcpServer.h"
#include "../net/EventLoop.h"
#include "ChatSession.h"

using namespace std;

enum CLIENT_TYPE
{
    CLIENT_TYPE_UNKOWN,
    CLIENT_TYPE_PC,
    CLIENT_TYPE_ANDROID,
};

struct StoredUserInfo
{
    int32_t         userid;
    std::string     username;
    std::string     password;
    std::string     nickname;
};

class ChatServer final {
public:
    ChatServer();
    ~ChatServer() = default;

    ChatServer(const ChatServer& rhs) = delete;
    ChatServer& operator = (const ChatServer& rhs) = delete;

    bool init(const char *ip,short port,EventLoop* loop);
    void uninit();

    void enableLogPackageBinary(bool enable);
    bool isLogPackageBinaryEnabled();

    void getSessions(std::list<std::shared_ptr<ChatSession>>& sessions);
    //用户id与clienttype唯一确定一个session
    bool getSessionByUserIdAndClientType(std::shared_ptr<ChatSession>& session,int32_t userid,int32_t clientType);

    bool getSessionsByUserId(std::list<std::shared_ptr<ChatSession>>& sessions,int32_t userid);
    //获知用户状态
    int32_t getUserStatusByUserId(int32_t userid);
    //获知用户客户端类型
    int32_t getUserClientTypeByUserId(int32_t userid);

private:
    void onConnected(std::shared_ptr<TcpConnection> conn);
    void onDisconnected(const std::shared_ptr<TcpConnection>& conn);
private:
    std::unique_ptr<TcpServer>                  m_server;
    std::list<std::shared_ptr<ChatSession>>     m_sessions;
    std::mutex                                  m_sessionMutex;
    std::atomic_int                             m_sessionId{};
    std::mutex                                  m_idMutex;
    std::atomic_bool                            m_logPackageBinary;
};