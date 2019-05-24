// Define.h
// ���ﶨ��Server����Ҫ�ӿڣ����������˿ڵȵȣ�
// ��Ȼ�Եã���ʹʲô��������Ҳ��һ�����ˡ�
// add by freeeyes
// 2008-12-23


#ifndef _SERVERMANAGER_H
#define _SERVERMANAGER_H

#include "BaseServerManager.h"
#include "ConnectAccept.h"
#include "ConsoleAccept.h"
#include "ClientReConnectManager.h"
#include "ReUDPManager.h"
#include "ReTTyClientManager.h"
#include "MessageQueueManager.h"
#include "ControlListen.h"

class Frame_Logging_Strategy;

class CServerManager : public IServerManager
{
public:
    CServerManager(void);
    ~CServerManager(void);

    CServerManager(const CServerManager& ar);

    CServerManager& operator = (const CServerManager& ar)
    {
        if (this != &ar)
        {
            ACE_UNUSED_ARG(ar);
        }

        return *this;
    }

    bool Init();
    bool Start();
    bool Close();


private:
    bool Init_Reactor(uint8 u1ReactorCount, uint8 u1NetMode);
    bool Run();

    bool Start_Tcp_Listen();                             //����TCP����
    bool Start_Udp_Listen();                             //����UDP����
    bool Start_Console_Tcp_Listen();                     //����Console TCP����
    void Multiple_Process_Start();                       //���������
    ConnectConsoleAcceptor  m_ConnectConsoleAcceptor;    //��̨��������
    Frame_Logging_Strategy* m_pFrameLoggingStrategy;     //�������
    CFrameCommand           m_objFrameCommand;           //�������
    ts_timer::CTimerThread  m_TSThread;                  //TS��ʱ��
};


typedef ACE_Singleton<CServerManager, ACE_Null_Mutex> App_ServerManager;

#endif
