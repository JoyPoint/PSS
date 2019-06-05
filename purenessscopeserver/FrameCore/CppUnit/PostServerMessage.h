#ifndef POSTSERVER_MESSAGE_H
#define POSTSERVER_MESSAGE_H

#include "IBuffPacket.h"
#include "ClientCommand.h"
#include "ClientReConnectManager.h"

//���ദ������͸����������߼�

#define RECV_BUFF_SIZE  MAX_BUFF_1024      //���ջ����С

class CPostServerData : public IClientMessage
{
public:
    CPostServerData()
    {
        m_u4ServerID         = 0;
        m_u2RecvBuffLength   = 0;
    }

    virtual ~CPostServerData()
    {
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
        if(NULL == pMessageBlockManager)
        {
            return false;
        }

        //���뷢�͵��ڴ�
        mbSend = pMessageBlockManager->Create(u4Len);

        if(NULL == mbSend)
        {
            OUR_DEBUG((LM_INFO, "[CPostServerData::Send_Format_data](%d)Format Create Data NULL(%d) OK.\n", m_u4ServerID, u4Len));
            return false;
        }

        //�����Ҫ��װ���ݣ�������д��Ĵ���
        //����ֻ��������ֻ���򵥵��ڴ濽������ʾ�����Ĳ�������
        memcpy_safe(pData, u4Len, mbSend->wr_ptr(), u4Len);
        mbSend->wr_ptr(u4Len);
        return true;
    }

    //�����ṩ��������ƴ���㷨������������ݰ�������RecvData����
    virtual bool Recv_Format_data(ACE_Message_Block* mbRecv, IMessageBlockManager* pMessageBlockManager, uint16& u2CommandID, ACE_Message_Block*& mbFinishRecv, EM_PACKET_ROUTE& emPacketRoute)
    {
        emPacketRoute = PACKET_ROUTE_SELF;
        u2CommandID = 0x1000;

        //�жϷ������ݿ��Ƿ�С��8���߳�����󻺳��С
        if(m_u2RecvBuffLength + mbRecv->length() < 8 || mbRecv->length() >= RECV_BUFF_SIZE)
        {
            OUR_DEBUG((LM_INFO, "[CPostServerData::Recv_Format_data]Get Data Error(%d).\n", mbRecv->length()));
            return false;
        }
        else
        {
            //ͳһ���뻺�壬���л����и������
            ACE_OS::memcpy(&m_szRecvBuffData[m_u2RecvBuffLength], mbRecv->rd_ptr(), mbRecv->length());
            m_u2RecvBuffLength += (uint16)mbRecv->length();
            mbRecv->rd_ptr(mbRecv->length());
        }



        //��������������ݰ��㷨
        uint32 u4Count = 0;
        memcpy_safe(&m_szRecvBuffData[0], sizeof(uint32), (char*)&u4Count, sizeof(uint32));

        uint32 u4RecvFinish = u4Count + sizeof(uint32);

        if (u4RecvFinish <= m_u2RecvBuffLength)
        {
            //�������ݰ�����
            mbFinishRecv = pMessageBlockManager->Create(u4RecvFinish);
            memcpy_safe(&m_szRecvBuffData[0], u4RecvFinish, mbFinishRecv->rd_ptr(), u4RecvFinish);
            mbFinishRecv->wr_ptr(u4RecvFinish);

            m_u2RecvBuffLength -= u4RecvFinish;
        }

        return true;
    }

    virtual bool RecvData(uint16 u2CommandID, ACE_Message_Block* mbRecv,  _ClientIPInfo const& objServerIPInfo)
    {
        //���ݰ��Ѿ���ȫ�������ﴦ������
        ACE_UNUSED_ARG(u2CommandID);
        ACE_UNUSED_ARG(mbRecv);
        ACE_UNUSED_ARG(objServerIPInfo);

        return true;
    }

    virtual void ReConnect(int nServerID)
    {
        OUR_DEBUG((LM_INFO, "[CPostServerData::ReConnect]nServerID=%d.\n", nServerID));
    }

    virtual bool ConnectError(int nError, _ClientIPInfo const& objServerIPInfo)
    {
        ACE_UNUSED_ARG(objServerIPInfo);
        OUR_DEBUG((LM_INFO, "[CPostServerData::ConnectError]nServerID=%d, nError=%d.\n", m_u4ServerID, nError));
        return true;
    }

    void SetServerID(uint32 u4ServerID)
    {
        m_u4ServerID = u4ServerID;
    }

private:
    uint32                     m_u4ServerID;
    char                       m_szRecvBuffData[RECV_BUFF_SIZE];  //���ջ����
    uint16                     m_u2RecvBuffLength;                //���ջ��峤��
};

#endif
