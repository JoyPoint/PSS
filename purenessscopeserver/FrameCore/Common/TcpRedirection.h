#ifndef _TCPREDIRECTION_H
#define _TCPREDIRECTION_H

//��������͸��ת�������ݽӿڡ�
//�����������һ����ܹ���
//add by freeeyes

#include "define.h"
#include "XmlConfig.h"
#include "BuffPacketManager.h"
#include "IMessageBlockManager.h"
#include "ClientCommand.h"
#include "IClientManager.h"
#include "IConnectManager.h"
#include "IDeviceHandler.h"
#include <unordered_map>

static const char FORWARD_XML[] = "forward.xml";

enum ENUM_FORWARD_TYPE
{
    ENUM_FORWARD_TCP_UNKNOW = 0,    //��ʼ��״̬
    ENUM_FORWARD_TCP_CLINET,        //�ͻ��˽���
    ENUM_FORWARD_TCP_S2S,           //������������
    ENUM_FORWARD_TCP_TTY            //TTY����
};

class CForwardInfo
{
public:
    CForwardInfo() {};

    string m_strSource;
    string m_strTarget;
};

class CForwardConnectInfo
{
public:
    CForwardConnectInfo() : m_u4ConnectID(0), m_emForwardType(ENUM_FORWARD_TCP_UNKNOW), m_u1ConnectState(0), m_pDeviceHandler(NULL)
    {
    };

    string            m_strSource;
    string            m_strTarget;
    uint32            m_u4ConnectID;
    ENUM_FORWARD_TYPE m_emForwardType;
    uint8             m_u1ConnectState;  //0�ǹرգ�1�Ǵ�
    IDeviceHandler*   m_pDeviceHandler;  //����ָ��
};

class CForwardManager
{
public:
    CForwardManager();

    void Close();

    int Init();

    string ConnectRegedit(const char* pIP, int nPort, ENUM_FORWARD_TYPE em_type, IDeviceHandler* pDeviceHandler);

    string ConnectRegedit(const char* pName, ENUM_FORWARD_TYPE em_type, IDeviceHandler* pDeviceHandler);

    void DisConnectRegedit(const char* pIP, int nPort, ENUM_FORWARD_TYPE em_type);

    void DisConnectRegedit(const char* pName, ENUM_FORWARD_TYPE em_type);

    void SendData(string strTarget, ACE_Message_Block* pmb);

    void AddForward(string strSource, string strTarget);

private:
    string Check_Connect_IP(const char* pName, ENUM_FORWARD_TYPE em_type, int ConnectState, IDeviceHandler* pDeviceHandler = NULL);

    IDeviceHandler* Get_Device_Handler(string strTarget);

    typedef unordered_map<string, CForwardConnectInfo*> mapForwardConnectList;
    vector<CForwardInfo> m_vecForwardInfo;          //��Ҫ��ע��ת���ӿ���Ϣ
    mapForwardConnectList m_mapForwardConnectList;
    int m_nActive;
};

typedef ACE_Singleton<CForwardManager, ACE_Null_Mutex> App_ForwardManager;

#endif
