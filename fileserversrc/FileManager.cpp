#include "FileManager.h"

#include "../base/AsyncLog.h"
#include "../base/Platform.h"

#include <memory>
#include <string.h>

bool FileManager::init(const char* base) {
    m_basedir = base;
    DIR *dp = opendir(base);

     if (dp == NULL)
    {
        LOGE("open base dir error, errno: %d, %s",  errno, strerror(errno));

        if (mkdir(base, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0)
            return true;

        LOGE("create base dir error, %s , errno: %d, %s", base, errno, strerror(errno));

        return false;
    }

    struct dirent* dirp;
    //struct stat filestat;
    while ((dirp = readdir(dp)) != NULL)
    {
        if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
            continue;


        //if (stat(dirp->d_name, &filestat) != 0)
        //{
        //    LOGW << "stat filename: [" << dirp->d_name << "] error, errno: " << errno << ", " << strerror(errno);
        //    continue;
        //}

        m_files.emplace_back(dirp->d_name);
        LOGI("filename: %s", dirp->d_name);
    }

    closedir(dp);

    return true;
}

void FileManager::addFile(const char* filename) {
    std::unique_lock<std::mutex> guard(m_mtx_file);
    m_files.emplace_back(filename);
}

bool FileManager::isFileExist(const char* filename) {
    std::unique_lock<std::mutex> guard(m_mtx_file);
    //察看缓存
    for(const auto& iter : m_files) {
        if(iter == filename) {
            return true;
        }
    }

    //在系统路径下寻找
    std::string path = m_basedir;
    path += filename;
    FILE* fp = fopen(path.c_str(),"r");
    if(fp != NULL) {
        fclose(fp);
        m_files.emplace_back(filename);
        return true;
    }

    return false;
}