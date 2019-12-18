#ifndef _LOGMANAGER_H
#define _LOGMANAGER_H

#include <stdio.h>
#include "ILogObject.h"
#include "ILogManager.h"
#include "BaseTask.h"

//������־��ĳ�
//һ��ʹ�������飬����־���÷ּ�����
//Ҳ������־���������
//add by freeeyes

class CLogBlockPool
{
public:
    CLogBlockPool();

    void Init(uint32 u4BlockSize, uint32 u4PoolCount);
    void Close();

    _LogBlockInfo* GetLogBlockInfo();                       //�õ�һ���������־��
    void ReturnBlockInfo(_LogBlockInfo* pLogBlockInfo);     //�黹һ���������־��

    uint32 GetBlockSize();

private:
    _LogBlockInfo* m_pLogBlockInfo;       //��־��
    uint32         m_u4MaxBlockSize;      //��־�ص����������
    uint32         m_u4PoolCount;         //��־���е���־�����
    uint32         m_u4CurrIndex;         //��־���е�ǰ���õ�����־��ID
};

class CLogManager : public ACE_Task<ACE_MT_SYNCH>, public ILogManager
{
public:
    CLogManager(void);

    int open ();
    virtual int svc(void);
    int Close();

    void Init(int nThreadCount = 1, int nQueueMax = MAX_MSG_THREADQUEUE, uint32 u4MailID = 0);
    int Start();
    int Stop();
    bool IsRun();

    int PutLog(_LogBlockInfo* pLogBlockInfo);
    int RegisterLog(IServerLogger* pServerLogger);
    int UnRegisterLog();

    void SetReset(bool blReset);

    void ResetLogData(uint16 u2LogLevel);

    //��ά���ӿ�
    uint32 GetLogCount();
    uint32 GetCurrLevel();

    uint16 GetLogID(uint16 u2Index);
    const char*  GetLogInfoByServerName(uint16 u2LogID);
    const char*  GetLogInfoByLogName(uint16 u2LogID);
    int    GetLogInfoByLogDisplay(uint16 u2LogID);
    uint16 GetLogInfoByLogLevel(uint16 u2LogID);

    //����д��־�Ľӿ�
    template <class... Args>
    int WriteLog_i(int nLogType, const char* fmt, Args&& ... args)
    {
        //����־��������ҵ�һ��������־��
        int nRet = 0;

        m_Logger_Mutex.acquire();
        _LogBlockInfo* pLogBlockInfo = m_objLogBlockPool.GetLogBlockInfo();

        if (NULL != pLogBlockInfo)
        {
            ACE_OS::snprintf(pLogBlockInfo->m_pBlock, m_objLogBlockPool.GetBlockSize() - 1, fmt, convert(std::forward<Args>(args))...);
            nRet = Update_Log_Block(nLogType, NULL, NULL, pLogBlockInfo);
        }

        m_Logger_Mutex.release();
        return nRet;
    };

    template <class... Args>
    int WriteToMail_i(int nLogType, uint32 u4MailID, const char* pTitle, const char* fmt, Args&& ... args)
    {
        int nRet = 0;
        m_Logger_Mutex.acquire();
        _LogBlockInfo* pLogBlockInfo = m_objLogBlockPool.GetLogBlockInfo();

        if (NULL != pLogBlockInfo)
        {
            ACE_OS::snprintf(pLogBlockInfo->m_pBlock, m_objLogBlockPool.GetBlockSize() - 1, fmt, convert(std::forward<Args>(args))...);
            nRet = Update_Log_Block(nLogType, &u4MailID, pTitle, pLogBlockInfo);
        }

        return nRet;
    };

    //����д��־�Ľӿ�
    virtual int WriteLogBinary(int nLogType, const char* pData, int nLen);

    virtual int WriteLog_r(int nLogType, const char* fmt, uint32 u4Len);

    virtual int WriteToMail_r(int nLogType, uint32 u4MailID, const char* pTitle, const char* fmt, uint32 u4Len);

private:
    bool Dispose_Queue();
    int ProcessLog(_LogBlockInfo* pLogBlockInfo);
    virtual int CloseMsgQueue();
    int Update_Log_Block(int nLogType, uint32* pMailID, const char* pTitle, _LogBlockInfo* pLogBlockInfo);

    //�ر���Ϣ������������
    ACE_Thread_Mutex                  m_mutex;
    ACE_Condition<ACE_Thread_Mutex>   m_cond;
    bool                              m_blRun;                    //��־ϵͳ�Ƿ�����
    bool                              m_blIsNeedReset;            //��־ģ��ȼ��������ñ�־
    bool                              m_blIsMail;                 //�Ƿ���Է����ʼ�
    int                               m_nThreadCount;             //��¼��־�̸߳�����ĿǰĬ����1
    int                               m_nQueueMax;                //��־�߳�����������и���
    CLogBlockPool                     m_objLogBlockPool;          //��־���
    ACE_Recursive_Thread_Mutex        m_Logger_Mutex;             //�߳���
    IServerLogger*                    m_pServerLogger;            //��־ģ��ָ��
};

typedef ACE_Singleton<CLogManager, ACE_Recursive_Thread_Mutex> AppLogManager;

#endif
