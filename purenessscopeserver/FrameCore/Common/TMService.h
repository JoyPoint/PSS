#pragma once

#include "ITMService.h"
#include "XmlOpeation.h"

class CTMService : public ITMService
{
public:
    CTMService();
    virtual ~CTMService();

    int Init(int nNeedLoad = 0);

    void Close();

    int AddMessage(string strName, unsigned long long nMessagePos, long sec, long usec, int _Message_id, void* _arg, Enum_Timer_Mode emTimerMode = Timer_Mode_Run_Once, IMessagePrecess* pMessagePrecess = NULL);
    void* DeleteMessage(string strName, unsigned long long nMessagePos);
    bool GetRun();

private:
    CTimerManager              timer_events_;
    int                        m_nTimerMaxCount;
    int                        m_nActive;
    CHashTable<CTimerInfo>     m_HashTimerList;
    CThreadQueueManager        m_ThreadQueueManager;
    unordered_map<int, int>    m_T2MList;
    unordered_map<int, int>    m_M2TList;

};