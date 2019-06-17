#ifndef _CLIENTCONNECTMANAGER_H
#define _CLIENTCONNECTMANAGER_H

#include "ace/Connector.h"
#include "ace/SOCK_Connector.h"

#include "TimerManager.h"
#include "BaseClientConnectManager.h"
#include "ReactorUDPClient.h"
#include "HashTable.h"
#include "XmlConfig.h"
#include "ConnectClient.h"

const uint32 RE_CONNECT_SERVER_TIMEOUT = 100 * 1000;
const uint32 WAIT_FOR_RECONNECT_FINISH = 5000;

typedef ACE_Connector<CConnectClient, ACE_SOCK_CONNECTOR> CConnectClientConnector;

class CReactorClientInfo
{
public:
    CReactorClientInfo();
    ~CReactorClientInfo();

    CReactorClientInfo(const CReactorClientInfo& ar);

    CReactorClientInfo& operator = (const CReactorClientInfo& ar)
    {
        if (this != &ar)
        {
            ACE_UNUSED_ARG(ar);
        }

        return *this;
    }

    bool Init(int nServerID, const char* pIP, int nPort, uint8 u1IPType, CConnectClientConnector* pReactorConnect, IClientMessage* pClientMessage, ACE_Reactor* pReactor, uint32 u4PacketParseID);  //��ʼ�����ӵ�ַ�Ͷ˿�
    void SetLocalAddr(const char* pIP, int nPort, uint8 u1IPType);                         //�󶨱��ص�IP�Ͷ˿�
    bool Run(bool blIsReady, EM_Server_Connect_State emState = SERVER_CONNECT_RECONNECT);  //��ʼ����
    bool SendData(ACE_Message_Block* pmblk);                                               //��������
    int  GetServerID();                                                                    //�õ�������ID
    bool Close();                                                                          //�رշ���������
    void SetConnectClient(CConnectClient* pConnectClient);                                 //��������״̬
    CConnectClient* GetConnectClient();                                                    //�õ�ProConnectClientָ��
    IClientMessage* GetClientMessage();                                                    //��õ�ǰ����Ϣ����ָ��
    ACE_INET_Addr GetServerAddr();                                                         //��÷������ĵ�ַ
    EM_Server_Connect_State GetServerConnectState();                                       //�õ���ǰ����״̬
    void SetServerConnectState(EM_Server_Connect_State objState);                          //���õ�ǰ����״̬
    void SetPacketParseID(uint32 u4PacketParseID);                                         //���ý�����ID
    uint32 GetPacketParseID();                                                             //��ý�����ID

private:
    ACE_INET_Addr              m_AddrLocal;              //���ص����ӵ�ַ������ָ����
    ACE_INET_Addr              m_AddrServer;             //Զ�̷������ĵ�ַ
    CConnectClient*            m_pConnectClient;         //��ǰ���Ӷ���
    CConnectClientConnector*   m_pReactorConnect;        //Connector���Ӷ���
    IClientMessage*            m_pClientMessage;         //�ص������࣬�ص����ش���ͷ������ݷ���
    uint32                     m_u4PacketParseID;        //�����õ�PacketParseID
    ACE_Reactor*               m_pReactor;               //��¼ʹ�õķ�Ӧ��
    bool                       m_blIsLocal;              //�Ƿ���Ҫ�ƶ����ض˿�
    int                        m_nServerID;              //Զ�̷�������ID
    EM_Server_Connect_State    m_emConnectState;         //����״̬
};

class CClientReConnectManager : public ACE_Event_Handler, public IClientManager
{
public:
    CClientReConnectManager(void);
    virtual ~CClientReConnectManager(void);

    CClientReConnectManager& operator = (const CClientReConnectManager& ar)
    {
        if (this != &ar)
        {
            ACE_UNUSED_ARG(ar);
        }

        return *this;
    }

    bool Init(ACE_Reactor* pReactor);
    virtual bool Connect(int nServerID, const char* pIP, int nPort, uint8 u1IPType, IClientMessage* pClientMessage);                                                             //���ӷ�����(TCP)
    virtual bool Connect(int nServerID, const char* pIP, int nPort, uint8 u1IPType, const char* pLocalIP, int nLocalPort, uint8 u1LocalIPType, IClientMessage* pClientMessage);  //���ӷ�����(TCP)��ָ�����ص�ַ

    virtual bool ConnectFrame(int nServerID, const char* pIP, int nPort, uint8 u1IPType, uint32 u4PacketParseID);                //����ָ���ķ�������������PacketParseID
    virtual bool ConnectFrame(int nServerID, const char* pIP, int nPort, uint8 u1IPType, const char* pLocalIP, int nLocalPort, uint8 u1LocalIPType, uint32 u4PacketParseID);    //����ָ���ķ�������������PacketParseID

    virtual bool ConnectUDP(int nServerID, const char* pIP, int nPort, uint8 u1IPType, EM_UDP_TYPE emType, IClientUDPMessage* pClientUDPMessage);                                //����һ��ָ��UDP�����ӣ�UDP��
    bool ReConnect(int nServerID);                                                                                             //��������һ��ָ���ķ�����(TCP)
    bool CloseByClient(int nServerID);                                                                                         //Զ�̱����ر�(TCP)
    virtual bool Close(int nServerID);                                                                                                 //�ر�����
    virtual bool CloseUDP(int nServerID);                                                                                              //�ر����ӣ�UDP��
    bool ConnectErrorClose(int nServerID);                                                                                     //���ڲ����������ʧ�ܣ���ProConnectClient����
    virtual bool SendData(int nServerID, char*& pData, int nSize, bool blIsDelete = true);                                              //��������
    virtual bool SendDataUDP(int nServerID, const char* pIP, int nPort, char*& pMessage, uint32 u4Len, bool blIsDelete = true);   //�������ݣ�UDP��
    bool SetHandler(int nServerID, CConnectClient* pConnectClient);                                                            //��ָ����CProConnectClient*�󶨸�nServerID
    virtual IClientMessage* GetClientMessage(int nServerID);                                                                           //���ClientMessage����
    virtual bool StartConnectTask(int nIntervalTime = CONNECT_LIMIT_RETRY);                                                            //�����Զ������Ķ�ʱ��
    virtual void CancelConnectTask();                                                                                                  //�ر�������ʱ��
    virtual void Close();                                                                                                              //�ر���������
    ACE_INET_Addr GetServerAddr(int nServerID);                                                                                //�õ�ָ����������Զ�̵�ַ������Ϣ
    bool SetServerConnectState(int nServerID, EM_Server_Connect_State objState);                                               //����ָ�����ӵ�����״̬
    virtual bool GetServerIPInfo(int nServerID, _ClientIPInfo& objServerIPInfo);                                                       //�õ�һ��nServerID��Ӧ��ServerIP��Ϣ
    virtual bool DeleteIClientMessage(IClientMessage* pClientMessage);                                                                 //ɾ��һ���������ڽ�����IClientMessage

    void GetConnectInfo(vecClientConnectInfo& VecClientConnectInfo);      //���ص�ǰ������ӵ���Ϣ��TCP��
    void GetUDPConnectInfo(vecClientConnectInfo& VecClientConnectInfo);   //���ص�ǰ������ӵ���Ϣ��UDP��
    virtual EM_Server_Connect_State GetConnectState(int nServerID);               //�õ�һ����ǰ����״̬
    virtual uint32 GetPacketParseID(int nServerID);                       //�õ���ǰ��PacketParseID

    virtual int handle_timeout(const ACE_Time_Value& current_time, const void* act = 0);               //��ʱ��ִ��

private:
    bool ConnectTcpInit(int nServerID, const char* pIP, int nPort, uint8 u1IPType, const char* pLocalIP, int nLocalPort, uint8 u1LocalIPType, IClientMessage* pClientMessage, CReactorClientInfo*& pClientInfo, uint32 u4PacketParseID = 0);
    bool ConnectUdpInit(int nServerID, CReactorUDPClient*& pReactorUDPClient);

public:
    CHashTable<CReactorClientInfo> m_objClientTCPList;            //TCP�ͻ�������
    CHashTable<CReactorUDPClient>  m_objClientUDPList;            //UDP�ͻ�������
    CConnectClientConnector        m_ReactorConnect;              //Reactor���ӿͻ��˶���
    ACE_Recursive_Thread_Mutex     m_ThreadWritrLock;             //�߳���
    int                            m_nTaskID;                     //��ʱ��⹤��
    ACE_Reactor*                   m_pReactor;                    //��ǰ�ķ�Ӧ��
    bool                           m_blReactorFinish;             //Reactor�Ƿ��Ѿ�ע��
    uint32                         m_u4ConnectServerTimeout;      //���Ӽ��ʱ��
    int32                          m_u4MaxPoolCount;              //���ӳص�����
};

typedef ACE_Singleton<CClientReConnectManager, ACE_Recursive_Thread_Mutex> App_ClientReConnectManager;
#endif
