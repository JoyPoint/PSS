#ifndef _THREADINFO_H
#define _THREADINFO_H

#include "define.h"

class _ThreadInfo
{
public:
    uint32              m_u4ThreadIndex;      //ϵͳ�߳�ID
    uint32              m_u4State;            //0Ϊû���������ڴ�����1Ϊ���ڴ����������ݡ�
    uint32              m_u4RecvPacketCount;  //���߳��½��հ��Ĵ�������
    uint32              m_u4CurrPacketCount;  //��λʱ���ڵİ���������
    uint16              m_u2CommandID;        //��ǰ����������ID
    uint16              m_u2PacketTime;       //ƽ�����ݰ�����ʱ��
    ACE_thread_t        m_u4ThreadID;         //�߳�ID
    ACE_Time_Value      m_tvUpdateTime;       //�߳���������ݵ�ʱ��
    ACE_Time_Value      m_tvCreateTime;       //�̴߳���ʱ��

    _ThreadInfo()
    {
        m_u4State           = THREAD_INIT;
        m_tvUpdateTime      = ACE_OS::gettimeofday();
        m_tvCreateTime      = ACE_OS::gettimeofday();
        m_u4RecvPacketCount = 0;
        m_u2CommandID       = 0;
        m_u4ThreadIndex     = 0;
        m_u2PacketTime      = 0;
        m_u4CurrPacketCount = 0;
        m_u4ThreadID        = 0;
    }

    ~_ThreadInfo()
    {
    }

    _ThreadInfo(const _ThreadInfo& ar)
    {
        (*this) = ar;
    }

    _ThreadInfo& operator = (const _ThreadInfo& ar)
    {
        if (this != &ar)
        {
            this->m_u4ThreadIndex = ar.m_u4ThreadIndex;
            this->m_tvUpdateTime = ar.m_tvUpdateTime;
            this->m_tvCreateTime = ar.m_tvCreateTime;
            this->m_u4State = ar.m_u4State;
            this->m_u4RecvPacketCount = ar.m_u4RecvPacketCount;
            this->m_u2CommandID = ar.m_u2CommandID;
            this->m_u2PacketTime = ar.m_u2PacketTime;
            this->m_u4CurrPacketCount = ar.m_u4CurrPacketCount;
        }

        return *this;
    }
};

class CThreadInfo
{
public:
    CThreadInfo(void);
    ~CThreadInfo(void);

    CThreadInfo(const CThreadInfo& ar);

    CThreadInfo& operator = (const CThreadInfo& ar)
    {
        if (this != &ar)
        {
            ACE_UNUSED_ARG(ar);
        }

        return *this;
    }

    void Init(int nCount);

    bool AddThreadInfo(uint32 u4ThreadID);
    bool AddThreadInfo(uint32 u4ThreadID, _ThreadInfo* pOrcThreadInfo);
    int  GetThreadCount();
    _ThreadInfo* GetThreadInfo(uint32 u4ThreadID);
    _ThreadInfo* GetThreadInfo();
    bool CloseThread(uint32 u4ThreadID);
    void Close();

private:
    _ThreadInfo** m_pAllThreadInfo;
    int           m_nThreadCount;
};
#endif