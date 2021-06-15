 /*
 *  消息缓存类， MsgCacheManager.h
 */
#include "../base/AsyncLog.h"
#include "MsgCacheManager.h"

MsgCacheManager::MsgCacheManager()
{

}

MsgCacheManager::~MsgCacheManager()
{

}

bool MsgCacheManager::addNotifyMsgCache(int32_t userid,const std::string& cache) {
    std::unique_lock<std::mutex> guard(m_mtNotifyMsgCache);
    NotifyMsgCache nc;
    nc.userid = userid;
    nc.notifymsg = std::move(cache);
    m_listNotifyMsgCache.emplace_back(std::move(nc));
    LOGI("append notify msg to cache, userid: %d ,m_mapNotifyMsgCache.size(): %d, cache length: %d", userid, m_listNotifyMsgCache.size(), cache.length());
    
    return true;
}

void MsgCacheManager::getNotifyMsgCache(int32_t userid,std::list<NotifyMsgCache>& cached) {
    std::unique_lock<std::mutex> guard(m_mtNotifyMsgCache);
    for(auto iter = m_listNotifyMsgCache.begin(); iter != m_listNotifyMsgCache.end(); ) {
        if(iter->userid == userid) {
            cached.emplace_back(std::move(*iter));
            iter = m_listNotifyMsgCache.erase(iter);
        }else {
            iter++;
        }
    }

     LOGI("get notify msg cache, userid: %d, , m_mapNotifyMsgCache.size(): %d, cached size: %d", userid, m_listNotifyMsgCache.size(), cached.size());
}

bool MsgCacheManager::addChatMsgCache(int32_t userid,const std::string& cache) {
    std::unique_lock<std::mutex> guard(m_mtChatMsgCache);
    ChatMsgCache c;
    c.userid = userid;
    c.chatmsg = std::move(cache);
    m_listChatMsgCache.emplace_back(std::move(c));
    LOGI("append chat msg to cache, userid: %d, m_listChatMsgCache.size(): , cache length: %d", userid, m_listChatMsgCache.size(), cache.length());

    return true;
}

void MsgCacheManager::getChatMsgCache(int32_t userid,std::list<ChatMsgCache>& cached) {
    std::unique_lock<std::mutex> guard(m_mtChatMsgCache);
    for(auto iter = m_listChatMsgCache.begin(); iter != m_listChatMsgCache.end() ; ) {
        if(iter->userid == userid) {
            cached.emplace_back(std::move(*iter));
            iter = m_listChatMsgCache.erase(iter);
        }else {
            iter++;
        }
    }

    LOGI("get chat msg cache, userid: %d, m_listChatMsgCache.size(): %d, cached size: %d", userid, m_listChatMsgCache.size(), cached.size());
}