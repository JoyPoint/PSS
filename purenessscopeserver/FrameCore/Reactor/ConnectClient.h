#ifndef _CONNECTCLIENT_H
#define _CONNECTCLIENT_H

//����ͻ�����Ϣ
//Reactor����ģʽ���������ݰ��������ݷ����߼��ദ��
//�������Proactor�µĿͻ��˷��ͣ���ʼ���reactor��ϵͳ��
//add by freeeyes
//2011-01-17

#include "define.h"

#include "ace/Reactor.h"
#include "ace/Synch.h"
#include "ace/SOCK_Stream.h"
#include "ace/Svc_Handler.h"
#include "ace/Reactor_Notification_Strategy.h"

#include "AceReactorManager.h"
#include "BaseConnectClient.h"
#include "LogManager.h"
#include "BaseHander.h"

class CConnectClient : public ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_MT_SYNCH>
{
public:
    CConnectClient(void);
    ~CConnectClient(void);

    virtual int open(void*);
    virtual int handle_input(ACE_HANDLE fd = ACE_INVALID_HANDLE);
    virtual int handle_close(ACE_HANDLE h, ACE_Reactor_Mask mask);
    virtual int handle_output(ACE_HANDLE fd = ACE_INVALID_HANDLE);

    void SetClientMessage(IClientMessage* pClientMessage); //������Ϣ���մ�����
    void SetServerID(int nServerID);                       //���õ�ǰ��ServerID
    int  GetServerID();                                    //��õ�ǰServerID
    void SetPacketParseInfoID(uint32 u4PacketParseInfoID); //����PacketParseID
    bool SendData(ACE_Message_Block* pmblk);
    void Close();

    bool GetTimeout(ACE_Time_Value const& tvNow);                 //��õ�ǰ���ݴ����Ƿ�ʱ

    void ClientClose();                                    //�����ر�
    _ClientConnectInfo GetClientConnectInfo();             //�õ���ǰ������Ϣ

    void Output_Debug_Data(ACE_Message_Block* pMbData, int nLogType, bool blLog = false);     //���DEBUG��Ϣ

private:
    int RecvData();                                                       //�������ݣ�����ģʽ
    int SendMessageGroup(uint16 u2CommandID, ACE_Message_Block* pmblk);   //�����յ����ݰ����������Ϣ����ģ��

    int Dispose_Recv_Data(ACE_Message_Block* pCurrMessage);               //������յ�������

    uint32                      m_u4SendSize;           //�����ֽ���
    uint32                      m_u4SendCount;          //�������ݰ���
    uint32                      m_u4RecvSize;           //�����ֽ���
    uint32                      m_u4RecvCount;          //�������ݰ���
    uint32                      m_u4CostTime;           //��Ϣ������ʱ��
    uint32                      m_u4CurrSize;           //��ǰ���յ����ֽ���
    uint32                      m_u4MaxPacketSize;      //�����հ���
    int                         m_nIOCount;             //��ǰIO�����ĸ���
    int                         m_nServerID;            //������ID
    uint8                       m_u1ConnectState;       //����״̬
    char                        m_szError[MAX_BUFF_500];
    ACE_INET_Addr               m_addrRemote;

    ACE_Recursive_Thread_Mutex  m_ThreadLock;
    IClientMessage*             m_pClientMessage;       //��Ϣ�������ָ��
    ACE_Message_Block*          m_pCurrMessage;         //��ǰ��MB����
    ACE_Time_Value              m_atvBegin;             //���ӽ���ʱ��

    EM_s2s                      m_ems2s;                //�Ƿ���Ҫ�ص�״̬
    ACE_Time_Value              m_atvRecv;              //���ݽ���ʱ��
    EM_Server_Recv_State        m_emRecvState;          //0Ϊδ�������ݣ�1Ϊ����������ɣ�2Ϊ�����������
    EM_CONNECT_IO_DISPOSE       m_emDispose;                 //����ģʽ����ܴ��� or ҵ����
    uint32                      m_u4PacketParseInfoID;       //��ܴ���ģ��ID
};
#endif
