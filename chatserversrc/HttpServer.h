/**
 * 聊天服务支持http请求, HttpServer.h
 */
#pragma once

#include <memory>
#include <mutex>
#include <list>
#include "../net/EventLoop.h"
#include "../net/TcpServer.h"

using namespace net;

class HttpSession;

class HttpServer final {
public:
    HttpServer() = default;
    ~HttpServer() = default;
    
    HttpServer(const HttpServer& rhs) = delete;
    HttpServer& operator = (const HttpServer& rhs) = delete;

public:
    bool init(const char* ip,short port,EventLoop* loop);
    void uninit();

    //新连接到来调用或连接断开，所以需要通过conn->connected()判断，一般只在主loop中调用
    void onConnected(std::shared_ptr<TcpConnection> conn);
    void onDisConnected(const std::shared_ptr<TcpConnection>& conn);

private:
    std::unique_ptr<TcpServer>                  m_server;
    std::list<std::shared_ptr<HttpSession>>     m_sessions;
    std::mutex                                  m_sessionMutex;
};