/** 
 * 即时通讯的业务逻辑都统一放在这里，BusinessLogic.h
 */
#ifndef __BUSSINESS_LOGIC_H__
#define __BUSSINESS_LOGIC_H__

#include <memory>
#include "../net/TcpConnection.h"

using namespace net;

class BussinessLogic final
{
private:
    BussinessLogic() = delete;
    ~BussinessLogic() = delete;

    BussinessLogic(const BussinessLogic& rhs) = delete;
    BussinessLogic& operator =(const BussinessLogic& rhs) = delete;

public:
    static void registerUser(const std::string& data, const std::shared_ptr<TcpConnection>& conn, bool keepalive, std::string& retData);
   // static void loginUser(const std::string& data,const std::shared_ptr<TcpConnection>& conn,int32_t sessionid,std::string& retData);
};



#endif //!__BUSSINESS_LOGIC_H__