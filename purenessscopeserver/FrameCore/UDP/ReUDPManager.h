#ifndef _REUDPMANAGER_H
#define _REUDPMANAGER_H

#include "ReactorUDPHander.h"
#include "IUDPConnectManager.h"

class CReUDPManager : public IUDPConnectManager
{
public:
    CReUDPManager() {}
    ~CReUDPManager() {}

    CReactorUDPHander* Create()
    {
        ACE_Guard<ACE_Recursive_Thread_Mutex> WGrard(m_ThreadWriteLock);
        CReactorUDPHander* pReactorUDPHandler = new CReactorUDPHander();

        if(NULL != pReactorUDPHandler)
        {
            m_vecReactorUDPHandler.push_back(pReactorUDPHandler);
            return pReactorUDPHandler;
        }

        return NULL;
    }

    void Close()
    {
        ACE_Guard<ACE_Recursive_Thread_Mutex> WGrard(m_ThreadWriteLock);

        for(CReactorUDPHander* pReactorUDPHandler : m_vecReactorUDPHandler)
        {
            pReactorUDPHandler->Close();
            SAFE_DELETE(pReactorUDPHandler);
        }
    }

    void GetClientConnectInfo(vecClientConnectInfo& VecClientConnectInfo)
    {
        ACE_Guard<ACE_Recursive_Thread_Mutex> WGrard(m_ThreadWriteLock);
        VecClientConnectInfo.clear();

        for(const CReactorUDPHander* pReactorUDPHandler : m_vecReactorUDPHandler)
        {
            if(NULL != pReactorUDPHandler)
            {
                VecClientConnectInfo.push_back(pReactorUDPHandler->GetClientConnectInfo());
            }
        }
    }

    void GetFlowInfo(uint32& u4FlowIn, uint32& u4FlowOut)
    {
        //�õ���ǰ����UDP�ĳ�������֮��
        ACE_Guard<ACE_Recursive_Thread_Mutex> WGrard(m_ThreadWriteLock);

        for (CReactorUDPHander* pReactorUDPHandler : m_vecReactorUDPHandler)
        {
            uint32 u4UdpFlowIn  = 0;
            uint32 u4UdpFlowOut = 0;

            if (NULL != pReactorUDPHandler)
            {
                pReactorUDPHandler->GetFlowInfo(u4UdpFlowIn, u4UdpFlowOut);
                u4FlowIn  += u4UdpFlowIn;
                u4FlowOut += u4UdpFlowOut;
            }
        }
    }

    bool SendMessage(int nConnectID, char*& pMessage, uint32 u4Len, const char* szIP, int nPort, bool blHead = true, uint16 u2CommandID = 0, bool blDlete = true)
    {
        ACE_Guard<ACE_Recursive_Thread_Mutex> WGrard(m_ThreadWriteLock);

        if(nConnectID >= (int)m_vecReactorUDPHandler.size())
        {
            OUR_DEBUG((LM_ERROR, "[CProUDPManager::SendMessage]Send (%d) UDP ConnectID is out of range.\n", nConnectID));
            return false;
        }

        CReactorUDPHander* pReactorUDPHandler = m_vecReactorUDPHandler[nConnectID];

        if(NULL != pReactorUDPHandler)
        {
            return pReactorUDPHandler->SendMessage(pMessage, u4Len, szIP, nPort, blHead, u2CommandID, blDlete);
        }
        else
        {
            return false;
        }
    }

    CReactorUDPHander* GetUDPHandle(uint8 u1Index)
    {
        if(u1Index >= (uint8)m_vecReactorUDPHandler.size())
        {
            return NULL;
        }
        else
        {
            return m_vecReactorUDPHandler[u1Index];
        }
    }

private:
    typedef vector<CReactorUDPHander*> vecReactorUDPHandler;
    vecReactorUDPHandler        m_vecReactorUDPHandler;
    ACE_Recursive_Thread_Mutex  m_ThreadWriteLock;       //�߳�������֤���̵߳��õ�һ����
};


typedef ACE_Singleton<CReUDPManager, ACE_Null_Mutex> App_ReUDPManager;

#endif
