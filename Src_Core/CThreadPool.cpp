#include "CThreadPool.h"
#include "CGlobal.h"
#include <iostream>
//#include <windows.h>

std::mutex CThreadPool::m_OnceMutex;//
std::mutex CThreadPool::m_sharedMutex;//
std::mutex CThreadPool::m_updateMutex;//
std::mutex CThreadPool::m_updateMutex1;//
std::mutex CThreadPool::m_MsgMutex;
std::mutex CThreadPool::m_RgnPicMutex;
std::mutex CThreadPool::m_ORGVokeMutex;
CThreadPool* CThreadPool::m_pSxThreadPool = nullptr;
void CThreadPool::addJob(std::function<void(void*)> foo)
{
  //CMutextLock lockMutex(&m_critical);
  CMutextLock lockMutex(&this->m_Mutex);
  this->m_vecFoo.push_back(foo);
  this->m_nFooSize++;
}

void CThreadPool::startOneJob(std::function<void(void*)> foo, void* extData/* = NULL*/)
{
  //CMutextLock lockMutex(&m_critical);
  CMutextLock lockMutex(&this->m_Mutex);
  this->m_vecFoo.push_back(foo);
  this->m_vecArg.push_back(extData);
  this->m_nFooSize++;
  //ReleaseSemaphore(m_hSemaphore, 1, 0);
  this->m_cond_var.notify_one();
}

int_32 CThreadPool::ThreadFunc(void* pParam)
{
  if (pParam == NULL) {
    return 0;
  }
  CThreadPool* pThis = (CThreadPool*)pParam;

  while (pThis->m_nStopAll == 0) {
    //int_32 aa1 = pThis->m_nFreeThreadSize;
    //printf("this_thread 5: %d %d\n", std::this_thread::get_id(), aa1);
    std::unique_lock<std::mutex> uniqueLock(pThis->m_Mutex);

    while (pThis->m_vecFoo.size() <= 0 && pThis->m_nStopAll == 0) {
      //WaitForSingleObject(pThis->m_hSemaphore, INFINITE);

      pThis->m_cond_var.wait(uniqueLock, [&] {
        if (pThis->m_vecFoo.empty())
          return false;
        return true;
      });
    }
    if (pThis->m_nStopAll) {
      break;
    }
    //CMutextLock(&pThis->m_critical);

    void* pArg = NULL;
    std::function<void(void*)> foo = NULL;
    //EnterCriticalSection(&pThis->m_critical);
    if (pThis->m_vecFoo.size() <= 0) {
      //LeaveCriticalSection(&pThis->m_critical);
      uniqueLock.unlock();
      continue;
    }
    {
        CMutextLock lockMutex(&pThis->m_MutexGetObj);
        foo = pThis->m_vecFoo.back();
        pArg = pThis->m_vecArg.back();
        pThis->m_vecFoo.pop_back();
        pThis->m_vecArg.pop_back();
        pThis->m_nFooSize--;
    }
    //pThis->m_vecFoo.erase(foo);
    //LeaveCriticalSection(&pThis->m_critical);
    pThis->m_nIsWorking = 1;
    //int_32 nnJob = pThis->m_nFreeThreadSize;
	//printf("正在处理[%d / %d]请稍后 ...\n", pThis->m_nFooSize, nnJob);

    uniqueLock.unlock();

    pThis->m_nFreeThreadSize++;//原子操作
    foo(pArg);
    pThis->m_nFreeThreadSize--;
	//nnJob = pThis->m_nFreeThreadSize;
	//printf("正在处理[jon :%d / ...\n",nnJob);
  }
  return 0;
}

void CThreadPool::setTip() {
  SX_DBG("set tip \n");
  this->m_nIsWorking = 0;
  this->m_c_start = clock();    //!< 单位为ms
  this->m_c_prev_time = this->m_c_start;
}

void CThreadPool::showTip(const char* szTipMsg) {
  SX_DBG("set tip 1\n");
  while (this->m_nIsWorking == 0);
  SX_DBG("set tip 2\n");
  this->m_c_end = clock();
  int_32 nnJob = this->m_nFreeThreadSize;
  while (nnJob > 0) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    this->m_c_end = clock();
    if (this->m_c_end - this->m_c_prev_time > 2000) {
      this->m_c_prev_time = this->m_c_end;
      nnJob = this->m_nFreeThreadSize;

      printf("\r[%s]正在处理[%d][耗时：%llu ms]，请稍后 ...", szTipMsg, nnJob, this->m_c_end - this->m_c_start);
    }
  }
  printf("\n[%s]已完成[耗时：%llu ms].\n", szTipMsg, this->m_c_end - this->m_c_start);
  this->m_nIsWorking = 0;
}
