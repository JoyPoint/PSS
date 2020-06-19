#ifndef _LOGICTHREADMANAGER_H
#define _LOGICTHREADMANAGER_H

#include "ace/Synch.h"
#include "ace/Malloc_T.h"
#include "ace/Singleton.h"
#include "ace/Thread_Mutex.h"

#include "define.h"
#include "HashTable.h"
#include "IMessageQueueManager.h"
#include "ObjectPoolManager.h"
#include "BaseTask.h"
#include "TimerManager.h"

const uint16 LOGICTHREAD_MAX_COUNT = 100;
const uint32 LOGICTHREAD_MESSAGE_MAX_COUNT = 2000;

const uint16 LOGICTHREAD_CREATE_TIMEWAIT = 1000;
const uint16 LOGICTHREAD_CLOSE_TIMEWAIT = 1000;

//�߼��̵߳���Ϣ��
class CLogicThreadMessage
{
public:
    CLogicThreadMessage()
    {
        //ָ���ϵҲ����������ֱ��ָ��������ʹ�õ�ʹ����ָ��
        m_pmbQueuePtr = new ACE_Message_Block(sizeof(CLogicThreadMessage*));

        CLogicThreadMessage** ppMessage = (CLogicThreadMessage**)m_pmbQueuePtr->base();
        *ppMessage = this;
    }

    void Close()
    {
        if (NULL != m_pmbQueuePtr)
        {
            m_pmbQueuePtr->release();
            m_pmbQueuePtr = NULL;
        }
    }

    void SetHashID(int nHasnID)
    {
        m_nHashID = nHasnID;
    }

    int GetHashID() const
    {
        return m_nHashID;
    }

    void Clear()
    {
        m_nMessageID = 0;
        m_pParam     = NULL;
    }

    ACE_Message_Block* GetQueueMessage()
    {
        return m_pmbQueuePtr;
    }

    int                 m_nHashID     = 0;
    int                 m_nMessageID  = 0;
    void*               m_pParam      = NULL;
    ACE_Message_Block*  m_pmbQueuePtr = NULL;        //��Ϣ����ָ���
};

//�߼��̲߳���
class CLogicThreadInfo
{
public:
    CLogicThreadInfo()
    {
    }

    int          m_nLogicThreadID = 0;
    int          m_nTimeout       = 0;
    ILogicQueue* m_pLogicQueue    = NULL;
};

//Message�����
class CLogicThreadMessagePool : public CObjectPoolManager<CLogicThreadMessage, ACE_Recursive_Thread_Mutex>
{
public:
    CLogicThreadMessagePool();

    static void Init_Callback(int nIndex, CLogicThreadMessage* pMessage);
    static void Close_Callback(int nIndex, CLogicThreadMessage* pMessage);

    CLogicThreadMessage* Create();
    bool Delete(CLogicThreadMessage* pMakePacket);
};

//����߼��߳�
class CLogicThread : public ACE_Task<ACE_MT_SYNCH>
{
public:
    CLogicThread();

    void Init(CLogicThreadInfo objThreadInfo);

    bool Start();

    virtual int handle_signal(int signum,
                              siginfo_t* = 0,
                              ucontext_t* = 0);

    int open();
    virtual int svc(void);
    int Close();

    bool CheckTimeout(ACE_Time_Value const& tvNow);

    bool PutMessage(int nMessageID, void* pParam);

    CLogicThreadInfo* GetThreadInfo();

private:
    int CloseMsgQueue();
    bool Dispose_Queue();                                  //��������

    CLogicThreadInfo                m_objThreadInfo;
    bool                            m_blRun             = false;
    ACE_Thread_Mutex                m_logicthreadmutex;
    ACE_Condition<ACE_Thread_Mutex> m_logicthreadcond;
    THREADSTATE                     m_u4ThreadState     = THREADSTATE::THREAD_INIT; //��ǰ�����߳�״̬
    ACE_Time_Value                  m_tvUpdateTime;                                 //�߳���������ݵ�ʱ��
    CLogicThreadMessagePool         m_MessagePool;                                  //��Ϣ��
};

//�߼��̹߳�����
class CMessageQueueManager : public IMessageQueueManager, public ACE_Task<ACE_MT_SYNCH>
{
public:
    CMessageQueueManager();

    virtual int handle_timeout(const ACE_Time_Value& tv, const void* arg);

    void Init();

    void Close();

    //�����߼��߳�
    int CreateLogicThread(int nLogicThreadID,
                          int nTimeout,
                          ILogicQueue* pLogicQueue);

    virtual int CreateLogicThread(ILogicQueue* pLogicQueue)
    {

        return CreateLogicThread(pLogicQueue->GetLogicThreadID(),
                                 pLogicQueue->GetTimeOut(),
                                 pLogicQueue);
    };

    //�ر��߼��߳�
    virtual int KillLogicThread(int nLogicThreadID);

    //�����Ϣ���߼��̵߳�ӳ���ϵ
    virtual int MessageMappingLogicThread(int nLogicThreadID, int nMessageID);

    //�����߳���Ϣ
    virtual int SendLogicThreadMessage(int nMessageID, void* arg);

    CHashTable<CLogicThread>     m_objThreadInfoList;
    CHashTable<CLogicThreadInfo> m_objMessageIDList;
    ACE_Recursive_Thread_Mutex   m_ThreadWriteLock;
    uint32                       m_u4TimerID = 0;
};

typedef ACE_Singleton<CMessageQueueManager, ACE_Null_Mutex> App_MessageQueueManager;

#endif
