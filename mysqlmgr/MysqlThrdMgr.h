#pragma once

#include "MysqlTask.h"
#include "MysqlThrd.h"

class CMysqlThrdMgr
{
public:
    CMysqlThrdMgr(int n = 9);
    virtual ~CMysqlThrdMgr(void);

public:
	bool init(const std::string& host, const std::string& user, const std::string& pwd, const std::string& dbname);
	void addTask(uint32_t dwHashID, IMysqlTask* poTask);
    void addTask(IMysqlTask* poTask) 
    { 
        return m_aoMysqlThreads[m_dwThreadsCount]->addTask(poTask); 
    }

	inline uint32_t getTableHashID(uint32_t dwHashID) const 
    { 
        return dwHashID % m_dwThreadsCount; 
    }

    bool processReplyTask(int32_t nCount);

    uint32_t getThreadsCount() 
    { 
        return m_dwThreadsCount; 
    }

protected:
	uint32_t m_dwThreadsCount;
    std::vector<std::shared_ptr<CMysqlThrd>>         m_aoMysqlThreads;
	//CMysqlThrd            m_aoMysqlThreads[m_dwThreadsCount+1];
};


