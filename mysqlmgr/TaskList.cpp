#include "TaskList.h"
#include <string.h>
#include "../base/AsyncLog.h"

CTaskList::CTaskList(uint16_t queueMaxSize/* = 1000*/,int concurrentGrading /*= 4*/,bool multiQueue/* false*/) 
: m_queueMaxSize(queueMaxSize),
m_concurrentGrading(concurrentGrading),
m_multiQueue(multiQueue),
m_uReadIndex(0),
m_uWriteIndex(0),
m_queueCurSize(0)
{
	if(m_multiQueue) {
		m_queues.reserve(m_concurrentGrading);
		for(auto i = 0;i < m_concurrentGrading; ++i) {
			m_queues.emplace_back(moodycamel::ConcurrentQueue<IMysqlTask*>());
		}
	}
}

CTaskList::~CTaskList(void)
{

}

size_t CTaskList::queueSize() {
	if(!m_multiQueue)
		return m_singleQueue.size_approx();
	else 
		return m_queueCurSize.load();
}

void CTaskList::push(IMysqlTask* poTask)
{
	if(!m_multiQueue) {
		while(!m_singleQueue.enqueue(poTask));
	} else {
		if(m_queueCurSize.load() < m_queueCurSize) {
			while(!m_queues[(m_uWriteIndex.fetch_add(1)) % m_concurrentGrading].enqueue(poTask));
		}
		++m_queueCurSize;
	}
}

IMysqlTask* CTaskList::pop()
{
	IMysqlTask* task;
	if(m_multiQueue) {
		task = pollingTake();
	}else {
		task = lockFreeTake();
	}
	return task;
}

IMysqlTask* CTaskList::lockFreeTake() {
	IMysqlTask* task;
	while(!m_singleQueue.try_dequeue(task));
	return task; 
}

IMysqlTask* CTaskList::pollingTake() {
	IMysqlTask* task;
	while(!m_queues[m_uReadIndex.fetch_add(1) % m_concurrentGrading].try_dequeue(task)) {
		if(m_queueCurSize.load() < 1) {
			std::this_thread::yield();
		}
	}
	--m_queueCurSize;
	return task;
}