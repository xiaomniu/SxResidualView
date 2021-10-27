#ifndef SXTHREADPOOL_H
#define SXTHREADPOOL_H

#include "Src_Core/DefType.h"
#include <thread>
#include <mutex>

#include <vector>
#include <functional>
#include <atomic>
#include <thread>
#include <condition_variable>

class CMutextLock {
public:
	CMutextLock(std::mutex * pMutex) {
		m_pMutex = pMutex;
		m_pMutex->lock();
	}
	~CMutextLock() {
		m_pMutex->unlock();
	}
	//CMutextLock(CRITICAL_SECTION* _critical) {
	//  m_critical = _critical;
	//  EnterCriticalSection(m_critical);
	//}
	//~CMutextLock() {
	//  LeaveCriticalSection(m_critical);
	//}
private:
	//CRITICAL_SECTION* m_critical;
	std::mutex * m_pMutex;
};

class CThreadPool;
class ThtreadItem {
public:
	ThtreadItem() {}
	~ThtreadItem() {}
public:
    CThreadPool* m_pThis = NULL;
	void* m_extData = NULL;
};


class CThreadPool
{
private:

    CThreadPool() {
		Init();
	}
    ~CThreadPool() {
		//DeleteCriticalSection(&m_critical);
		//CloseHandle(m_hSemaphore);
		this->m_nStopAll = 1;
		this->m_cond_var.notify_all();
		for (auto iter = this->m_vecThread.begin(); iter != this->m_vecThread.end(); iter++) {
			if ((*iter)->joinable()) {
				(*iter)->join();
			}
		}
		for (auto iter = this->m_vecThread.begin(); iter != this->m_vecThread.end(); iter++) {
			delete *iter;
			*iter = nullptr;
		}
		this->m_vecThread.clear();
	}
    static CThreadPool* m_pSxThreadPool;
	class CDestroySelf {
	public:
	  CDestroySelf(){}
	  ~CDestroySelf() {
	    delete m_pSxThreadPool;
	    m_pSxThreadPool = nullptr;
	  }
	};
	CDestroySelf m_destroySelf;

public:
    int_32 m_nIsWorking = 0;
    int_32 m_nStopAll = 0;
	std::mutex m_Mutex;
    std::mutex m_MutexGetObj;
	static std::mutex m_OnceMutex;
	static std::mutex m_sharedMutex;
	static std::mutex m_updateMutex;
	static std::mutex m_updateMutex1;
	static std::mutex m_MsgMutex;
	static std::mutex m_RgnPicMutex;
	static std::mutex m_ORGVokeMutex;
	std::condition_variable m_cond_var;

	//CRITICAL_SECTION m_critical;
	//HANDLE m_hSemaphore;
public:
    static CThreadPool* getInstance() {
		if (m_pSxThreadPool == nullptr) {
			CMutextLock lockMutex(&m_OnceMutex);
			if (m_pSxThreadPool == nullptr) {
                m_pSxThreadPool = new CThreadPool;
			}
		}
		return m_pSxThreadPool;
	}


    bool Init(int_32 nPoolSize = 64) {
		if (m_nPoolSize > 0)//不允许重复 设置线程数量
			return false;
		//m_nPoolSize = nPoolSize;//暂时不开放超过15的自定义线程数量
		m_nPoolSize = nPoolSize;
		//if(nPoolSize > 15)
		//  m_nPoolSize = 15;
		//InitializeCriticalSection(&m_critical);
		//m_hSemaphore = CreateSemaphore(NULL, 0, m_nPoolSize, NULL);

		//DWORD dwTid = 0;
		std::thread* pThread = nullptr;
        for (int_32 i = 0; i < m_nPoolSize; i++) {
			//CreateThread(NULL, 0, ThreadFunc, (void*)this, 0, &dwTid);
			pThread = new std::thread(ThreadFunc, this);
			m_vecThread.push_back(pThread);
		}
		return true;
	}
	std::vector<std::thread*> m_vecThread;

    std::atomic<int_32> m_nFreeThreadSize = { 0 };
    int_32 m_nPoolSize = 0;
    int_32 m_nFooSize = 0;
	std::vector<std::function<void(void*)>> m_vecFoo;
	std::vector<void*> m_vecArg;

	time_t m_c_prev_time;
	time_t m_c_start, m_c_end;

public:

	void addJob(std::function<void(void*)> foo);
	void startOneJob(std::function<void(void*)> foo, void* extData = NULL);
	//void startOneJob(std::function<void()> foo);
    static int_32 ThreadFunc(void* pParam);

	void setTip();
	void showTip(const char* szTipMsg);

	time_t m_c_begin, m_c_ended;
	void beginCount() {
		;
	}
	void endedCount() {
		;
	}
};

#endif // !1
