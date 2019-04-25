#ifndef _CONSOLEHANDLE_H
#define _CONSOLEHANDLE_H

//�����ڶԷ��������Ƶ�ģ�飬����Զ�̿����������
//������汾��һֱ��ӣ���������ſ�ʼ�㣬��Ҫ���Լ�������ξ������ٸ��Լ�������ɡ�
//add by freeeyes
#include "define.h"

#include "ace/Reactor.h"
#include "ace/Svc_Handler.h"
#include "ace/Synch.h"
#include "ace/SOCK_Acceptor.h"
#include "ace/Reactor_Notification_Strategy.h"

#include "AceReactorManager.h"
#include "MessageService.h"
#include "IConnectManager.h"
#include "BaseConsoleHandle.h"
#include "PacketConsoleParse.h"

class CConsoleHandler : public ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_MT_SYNCH>
{
public:
    CConsoleHandler(void);
    ~CConsoleHandler(void);

    CConsoleHandler& operator = (const CConsoleHandler& ar)
    {
        if (this != &ar)
        {
            ACE_UNUSED_ARG(ar);
        }

        return *this;
    }

    //��д�̳з���
    virtual int open(void*);                                                //�û�����һ������
    virtual int handle_input(ACE_HANDLE fd = ACE_INVALID_HANDLE);
    virtual int handle_close(ACE_HANDLE h, ACE_Reactor_Mask mask);

    bool SendMessage(IBuffPacket* pBuffPacket, uint8 u1OutputType);

    void Close(int nIOCount = 1);
    bool ServerClose();

    const char* GetError();
    void        SetConnectID(uint32 u4ConnectID);
    uint32      GetConnectID();
    uint8       GetConnectState();                                           //�õ�����״̬
    uint8       GetSendBuffState();                                          //�õ�����״̬
    uint8       GetIsClosing();                                              //�����Ƿ�Ӧ�ùر�

private:
    bool CheckMessage();                                                      //�������յ�����
    bool PutSendPacket(ACE_Message_Block* pMbData);                           //��������
    void Clear_PacketParse();                                                 //��������Ҫ��PacketParse
    bool CompareConsoleClinetIP(const char* pIP);                             //���һ��Console���ӵ�IP�Ƿ���ָ���������б���


    uint32                     m_u4HandlerID;                  //��Hander����ʱ��ID
    uint32                     m_u4ConnectID;                  //���ӵ�ID
    uint32                     m_u4AllRecvCount;               //��ǰ���ӽ������ݰ��ĸ���
    uint32                     m_u4AllSendCount;               //��ǰ���ӷ������ݰ��ĸ���
    uint32                     m_u4AllRecvSize;                //��ǰ���ӽ����ֽ�����
    uint32                     m_u4AllSendSize;                //��ǰ���ӷ����ֽ�����
    uint32                     m_u4SendThresHold;              //���ͷ�ֵ(��Ϣ���ĸ���)
    uint32                     m_u4SendCheckTime;              //���ͼ��ʱ��ķ�ֵ
    uint32                     m_u4CurrSize;                   //��ǰMB�����ַ�����
    int                        m_nIOCount;                     //��ǰIO�����ĸ���
    uint16                     m_u2SendQueueMax;               //���Ͷ�����󳤶�
    uint16                     m_u2SendCount;                  //��ǰ���ݰ��ĸ���
    uint16                     m_u2MaxConnectTime;             //���ʱ�������ж�
    uint8                      m_u1ConnectState;               //Ŀǰ���Ӵ���״̬
    uint8                      m_u1SendBuffState;              //Ŀǰ�������Ƿ��еȴ����͵�����
    uint8                      m_u1IsClosing;                  //�Ƿ�Ӧ�ùر� 0Ϊ��1Ϊ��
    char                       m_szError[MAX_BUFF_500];
    ACE_INET_Addr              m_addrRemote;
    ACE_Time_Value             m_atvConnect;
    ACE_Time_Value             m_atvInput;
    ACE_Time_Value             m_atvOutput;
    ACE_Time_Value             m_atvSendAlive;
    ACE_Recursive_Thread_Mutex m_ThreadLock;
    _TimerCheckID*             m_pTCClose;
    CConsolePacketParse*       m_pPacketParse;                 //���ݰ�������
    ACE_Message_Block*         m_pCurrMessage;                 //��ǰ��MB����
};

#endif