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
#include <unordered_map>

using namespace std;

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
    CForwardConnectInfo() : m_u4ConnectID(0), m_emForwardType(ENUM_FORWARD_TCP_UNKNOW), m_u1ConnectState(0)
    {
    };

    string            m_strSource;
    string            m_strTarget;
    uint32            m_u4ConnectID;
    ENUM_FORWARD_TYPE m_emForwardType;
    uint8             m_u1ConnectState;  //0�ǹرգ�1�Ǵ�
};

class CForwardManager
{
public:
    CForwardManager();

    void Close();

    int Init(int nNeedLoad = 0);

    void ConnectRegedit(const char* pIP, int nPort, ENUM_FORWARD_TYPE em_type);

    void ConnectRegedit(const char* pName, ENUM_FORWARD_TYPE em_type);

    void DisConnectRegedit(const char* pIP, int nPort, ENUM_FORWARD_TYPE em_type);

    void DisConnectRegedit(const char* pName, ENUM_FORWARD_TYPE em_type);

    void SendData(const char* pIP, int nPort, ACE_Message_Block* pmb);

    void RecvData(const char* pIP, int nPort, ACE_Message_Block* pmb);

private:
    void Check_Connect_IP(const char* pName, ENUM_FORWARD_TYPE em_type, int ConnectState);

private:
    typedef unordered_map<string, CForwardConnectInfo*> mapForwardConnectList;

    vector<CForwardInfo> m_vecForwardInfo;          //��Ҫ��ע��ת���ӿ���Ϣ
    mapForwardConnectList m_mapForwardConnectList;
    int m_nActive;
    int m_nNeedLoad;

};

typedef ACE_Singleton<CForwardManager, ACE_Null_Mutex> App_ForwardManager;

#endif
