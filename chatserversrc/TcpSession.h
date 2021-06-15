 /**
 * TcpSession.h
 **/
#pragma once 

#include <memory>
#include "../net/TcpConnection.h"

using namespace net;

//为了让业务与逻辑分离，实际应该新增子类继承自TcpSession,让TcpSession中仅含逻辑代码
//其子类存放业务代码
class TcpSession {
public:
    TcpSession(const std::weak_ptr<TcpConnection>& tmpConn);
    ~TcpSession();

    TcpSession(const TcpSession& rhs) = delete;
    TcpSession& operator =(const TcpSession& rhs) = delete;

    std::shared_ptr<TcpConnection> getConnectionPtr() {
        if(tmpConn_.expired()) {
            return NULL;
        }

        return tmpConn_.lock();
    } 

    void send(int32_t cmd,int32_t seq,const std::string& data);

    void send(int32_t cmd,int32_t seq,const char* data,int32_t dataLength);

    void send(const std::string& p);

    void send(const char* p,int32_t length);
private:
    void sendPackage(const char* p,int32_t length);

protected:
    //TcpSession引用TcpConnection类必须是弱指针，因为TcpConnection可能因网络出错自行销毁，此时session也应随之消亡
    std::weak_ptr<TcpConnection>    tmpConn_;
};