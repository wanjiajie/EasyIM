#pragma once

#include <string>
#include <mutex>
#include <list>

class FileManager final {
public:
    FileManager() = default;
    ~FileManager() = default;

    FileManager(const FileManager&) = delete;
    FileManager& operator = (const FileManager&) = delete;

    bool init(const char* base);
    bool isFileExist(const char* fileName);
    void addFile(const char* fileName);
private:
    //缓存
    std::list<std::string>          m_files;        //均以md5串格式存储
    std::mutex                      m_mtx_file;
    std::string                     m_basedir; 
};