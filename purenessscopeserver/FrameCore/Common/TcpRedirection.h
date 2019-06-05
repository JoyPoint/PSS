#ifndef _TCPREDIRECTION_H
#define _TCPREDIRECTION_H

//��������͸��ת�������ݽӿڡ�
//�����������һ����ܹ���
//add by freeeyes

#include "define.h"
#include "XmlConfig.h"
#include "HashTable.h"
#include "BuffPacketManager.h"
#include "IMessageBlockManager.h"
#include "ClientCommand.h"
#include "IClientManager.h"
#include "IConnectManager.h"

#define MAX_CONNECT_REDIRECTION_COUNT  5

class CRedirectionData : public IClientMessage
{
public:
    CRedirectionData() : m_u4ServerID(0), m_u1Mode(0), m_u1ConnectState(0), m_pConnectManager(NULL)
    {
    }

    virtual ~CRedirectionData()
    {
    }

    CRedirectionData(const CRedirectionData& ar)
    {
        (*this) = ar;
    };

    CRedirectionData& operator = (const CRedirectionData& ar)
    {
        if (this != &ar)
        {
            ACE_UNUSED_ARG(ar);
        }

        return *this;
    }

    void SetMode(uint8 u1Mode)
    {
        m_u1Mode = u1Mode;
    }

    uint8 GetMode()
    {
        return m_u1Mode;
    }

    void SetConnectState(uint8 u1ConnectState)
    {
        m_u1ConnectState = u1ConnectState;
    }

    void SetConnectManager(IConnectManager* pConnectManager)
    {
        m_pConnectManager = pConnectManager;
    }

    virtual bool Need_Send_Format()
    {
        //�����Ҫ���÷�����װ�����������ﷵ��true
        //����Ҫ�Ļ����ﷵ��false��Send_Format_data�����Ͳ��ᱻ����
        return true;
    }

    //��������ڼ䷢�����ݵ���װ
    virtual bool Send_Format_data(char* pData, uint32 u4Len, IMessageBlockManager* pMessageBlockManager, ACE_Message_Block*& mbSend)
    {
        //�жϻ�����Ƿ���ڣ�����������򷵻�ʧ��
        if (NULL == pMessageBlockManager)
        {
            return false;
        }

        //���뷢�͵��ڴ�
        mbSend = pMessageBlockManager->Create(u4Len);
        memcpy_safe(pData, u4Len, mbSend->wr_ptr(), u4Len);
        mbSend->wr_ptr(u4Len);
        return true;
    }

    //�����ṩ��������ƴ���㷨������������ݰ�������RecvData����
    virtual bool Recv_Format_data(ACE_Message_Block* mbRecv, IMessageBlockManager* pMessageBlockManager, uint16& u2CommandID, ACE_Message_Block*& mbFinishRecv, EM_PACKET_ROUTE& emPacketRoute)
    {
        ACE_UNUSED_ARG(pMessageBlockManager);
        emPacketRoute = PACKET_ROUTE_SELF;

        if (mbRecv->length() == 0)
        {
            return false;
        }

        u2CommandID = 0x1000;

        mbFinishRecv = pMessageBlockManager->Create((uint32)mbRecv->length());
        memcpy_safe(mbRecv->rd_ptr(), (uint32)mbRecv->length(), mbFinishRecv->wr_ptr(), (uint32)mbRecv->length());
        mbFinishRecv->wr_ptr(mbRecv->length());
        mbRecv->rd_ptr(mbRecv->length());

        return true;
    }

    virtual bool RecvData(uint16 u2CommandID, ACE_Message_Block* mbRecv, _ClientIPInfo const& objServerIPInfo)
    {
        //���ݰ��Ѿ���ȫ�������ﴦ������
        ACE_UNUSED_ARG(u2CommandID);
        ACE_UNUSED_ARG(objServerIPInfo);

        if (NULL == m_pConnectManager || 0 == m_u1Mode)
        {
            return true;
        }

        IBuffPacket* pBuffPacket = App_BuffPacketManager::instance()->Create(__FILE__, __LINE__);

        pBuffPacket->WriteStream((char* )mbRecv->rd_ptr(), (uint32)mbRecv->length());

        //������ת����ָ����ConnectID
        return m_pConnectManager->PostMessage(m_u4ServerID,
                                              pBuffPacket,
                                              SENDMESSAGE_JAMPNOMAL,
                                              0,
                                              PACKET_SEND_IMMEDIATLY,
                                              PACKET_IS_FRAMEWORK_RECYC,
                                              0);
    }

    virtual void ReConnect(int nServerID)
    {
        OUR_DEBUG((LM_INFO, "[CRedirectionData::ReConnect]nServerID=%d.\n", nServerID));
    }

    virtual bool ConnectError(int nError, _ClientIPInfo const& objServerIPInfo)
    {
        ACE_UNUSED_ARG(objServerIPInfo);
        OUR_DEBUG((LM_INFO, "[CRedirectionData::ConnectError]nServerID=%d, nError=%d.\n", m_u4ServerID, nError));

        if (1 == m_u1ConnectState)
        {
            //�Ͽ��ͻ���
            m_pConnectManager->CloseConnect(m_u4ServerID);
        }

        return true;
    }

    void SetServerID(uint32 u4ServerID)
    {
        m_u4ServerID = u4ServerID;
    }

private:
    uint32                     m_u4ServerID;
    uint8                      m_u1Mode;            //0 ����Ӧ����, 1 ��Ӧ����
    uint8                      m_u1ConnectState;    //0 ����Զ������״̬ͬ�� 1 ��Զ������״̬ͬ��
    IConnectManager*           m_pConnectManager;

};

class CTcpRedirection
{
public:
    CTcpRedirection();
    ~CTcpRedirection();

    CTcpRedirection(const CTcpRedirection& ar);

    CTcpRedirection& operator = (const CTcpRedirection& ar)
    {
        if (this != &ar)
        {
            ACE_UNUSED_ARG(ar);
        }

        return *this;
    }

    void Close();

    void Init(std::vector<xmlTcpRedirection::_RedirectionInfo> vec, uint32 u4MaxHandlerCount, IClientManager* pClientManager, IConnectManager* pConnectManager);

    void ConnectRedirect(uint32 u4SrcPort, uint32 u4ConnectID);

    void DataRedirect(uint32 u4ConnectID, ACE_Message_Block* mb);

    void CloseRedirect(uint32 u4ConnectID);

    bool GetMode(uint32 u4LocalPort);

private:
    CHashTable<xmlTcpRedirection::_RedirectionInfo> m_objRedirectList;          //ת���������ӿ��б�
    CHashTable<CRedirectionData>                    m_objRedirectConnectList;   //ת����������Ӧ���ӽṹ
    uint16                                          m_u2Count;
    IClientManager*                                 m_pClientManager;
    IConnectManager*                                m_pConnectManager;
};

typedef ACE_Singleton<CTcpRedirection, ACE_Null_Mutex> App_TcpRedirection;

#endif
