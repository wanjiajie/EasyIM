#pragma once

#include "MysqlTask.h"
#include <stdint.h>
#include <atomic>
#include <vector>
#include "../base/Concurrentqueue.h"

//#define MAX_TASK_NUM 1000

class CTaskList
{
public:
    CTaskList(uint16_t queueMaxSize = 1000, int concurrentGrading = 4, bool multiQueue = true);
    ~CTaskList();

    void push(IMysqlTask* poTask);                  // �߼��߳��޸� 
    IMysqlTask* pop();				                // ���ݿ��߳��޸� 

    IMysqlTask* lockFreeTake();
    IMysqlTask* pollingTake();

    size_t queueSize();

private:
    uint16_t            m_queueMaxSize;
    int                 m_concurrentGrading;
    bool                m_multiQueue;

    std::atomic_int     m_uReadIndex;               // ���ݿ��߳��޸� 
    std::atomic_int     m_uWriteIndex;              // �߼��߳��޸�  
    std::atomic_int     m_queueCurSize; 
	//IMysqlTask*         m_pTaskNode[MAX_TASK_NUM];
    moodycamel::ConcurrentQueue<IMysqlTask*>                m_singleQueue;
    std::vector<moodycamel::ConcurrentQueue<IMysqlTask*>>   m_queues;
};



