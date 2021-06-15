#pragma once 

#include "../net/Buffer.h"
#include "TcpSession.h"

class FileSession :  public TcpSession {
public:
    FileSession(const std::shared_ptr<TcpConnection>& conn,const char* basedir);
    virtual ~FileSession();
    
    FileSession(const FileSession& rhs) = delete;
    FileSession& operator = (const FileSession& rhs) = delete;

    void onRead(const std::shared_ptr<TcpConnection>& conn,Buffer* pBuffer,Timestamp receivTime);
private:
    bool process(const std::shared_ptr<TcpConnection>& conn,const char* inbuf,size_t length);

    bool onUploadFileResponse(const std::string& filemd5, int64_t offset, int64_t filesize, const std::string& filedata, const std::shared_ptr<TcpConnection>& conn);
    bool onDownloadFileResponse(const std::string& filemd5, int32_t clientNetType, const std::shared_ptr<TcpConnection>& conn);

    void releaseFile();
private:
    int32_t             m_id;   //session id 
    int32_t             m_seq;  //当前session序列号

    //文件
    FILE*               m_fp{};
    int64_t             m_currentDownloadFileOffset{};
    int64_t             m_currentDownloadFileSize{};
    std::string         m_FileBaseDir;
    bool                m_bFileUpLoading;
};