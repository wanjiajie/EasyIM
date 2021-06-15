 /**
 * TcpSession.h
 **/
#include "TcpSession.h"
#include "../base/AsyncLog.h"
#include "../base/Singleton.h"
#include "../net/ProtocolStream.h" 
#include "../zlib1.2.11/ZlibUtil.h"
#include "../net/TcpConnection.h"
#include "FileMsg.h"


using namespace net;

TcpSession::TcpSession(const std::weak_ptr<TcpConnection>& tmpConn) 
:tmpConn_(tmpConn) 
{

}

TcpSession::~TcpSession() {

}

void TcpSession::send(int32_t cmd, int32_t seq, int32_t errorcode, const std::string& filemd5, int64_t offset, int64_t filesize, const std::string& filedata) {
    std::string outbuf;
    BinaryStreamWriter writeStream(&outbuf);
    writeStream.WriteInt32(cmd);
    writeStream.WriteInt32(seq);
    writeStream.WriteInt32(errorcode);
    writeStream.WriteString(filemd5);
    writeStream.WriteInt64(offset);
    writeStream.WriteInt64(filesize);
    writeStream.WriteString(filedata);

    writeStream.Flush();

    sendPackage(outbuf.c_str(),outbuf.length());
}


void TcpSession::sendPackage(const char* body, int64_t bodyLength) {
    std::string strPackageData;
    file_msg_header header{bodyLength};
    
    strPackageData.append((const char*)(&header),sizeof(header));
    strPackageData.append(body,bodyLength);

    if(tmpConn_.expired()) {
        //connection先于Session被关闭
        LOGE("Tcp Connection is destroyed , TcpSession is still alive");
        return;
    }

    std::shared_ptr<TcpConnection> conn = tmpConn_.lock();
    if(conn) {
        LOGI("Send data,package length: %d,body length: %d",strPackageData.length(),bodyLength);
        conn->send(strPackageData.c_str(),strPackageData.length());
    }

}