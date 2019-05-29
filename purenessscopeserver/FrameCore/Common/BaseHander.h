#ifndef _BASEREACTORHANDLER_H
#define _BASEREACTORHANDLER_H

#include "ace/Task_T.h"

#include "define.h"
#include "SendMessage.h"
#include "MessageBlockManager.h"
#include "LoadPacketParse.h"
#include "PacketParsePool.h"
#include "MakePacket.h"
#include "FileTest.h"
#include "SendCacheManager.h"
#include "ace/SOCK_Dgram.h"
#include "XmlConfig.h"
#include "TcpRedirection.h"

//���е�handler�õ��Ĺ�������
//add by freeeyes


//�����ڴ�
void Recovery_Message(bool blDelete, char*& pMessage);

//Udp_Common_Send_Message�Ĵ���
class _Send_Message_Param
{
public:
    uint32 m_u4PacketParseInfoID;
    char*  m_pIP;
    int    m_nPort;
    bool   m_blHead;
    uint16 m_u2CommandID;
    bool   m_blDlete;
    uint32 m_u4Len;

    _Send_Message_Param()
    {
        m_u4PacketParseInfoID = 0;
        m_pIP = NULL;
        m_nPort = 0;
        m_blHead = true;
        m_u2CommandID = 0;
        m_blDlete = false;
        m_u4Len = 0;
    }
};

//udp�����������ݰ��ϳɺ���
bool Udp_Common_Send_Message(_Send_Message_Param obj_Send_Message_Param, ACE_INET_Addr& AddrRemote, char*& pMessage,
                             ACE_Message_Block*& pMbData, ACE_SOCK_Dgram& skRemote);

//udp������Ϣͷ����
bool Udp_Common_Recv_Head(ACE_Message_Block* pMBHead, CPacketParse* pPacketParse, uint32 u4PacketParseInfoID, uint32 u4Len);

//udp������Ϣ�庯��
bool Udp_Common_Recv_Body(ACE_Message_Block* pMBBody, CPacketParse* pPacketParse, uint32 u4PacketParseInfoID);

//udp����Ϣ����
bool Udp_Common_Recv_Stream(ACE_Message_Block* pMbData, CPacketParse* pPacketParse, uint32 u4PacketParseInfoID);

//�ύudp���ݵ������߳�
bool Udp_Common_Send_WorkThread(CPacketParse*& pPacketParse, ACE_INET_Addr addrRemote, ACE_INET_Addr addrLocal, ACE_Time_Value& tvCheck);

//�������ݻ���
void Recovery_Common_BuffPacket(bool blDelete, IBuffPacket* pBuffPacket);

//������Ϣ���ع����߳�
void Tcp_Common_Send_Message_Error(bool blDelete, IBuffPacket* pBuffPacket);

//TCP����Ϣ����
uint8 Tcp_Common_Recv_Stream(uint32 u4ConnectID, ACE_Message_Block* pMbData, CPacketParse* pPacketParse, uint32 u4PacketParseInfoID);

//�����ݷ����빤���߳���Ϣ����
void Send_MakePacket_Queue(uint32 u4ConnectID, uint32 u4PacketParseID, CPacketParse* pPacketParse, uint8 u1Option,
                           ACE_INET_Addr& addrRemote, const char* pLocalIP,
                           uint32 u4LocalPort, EM_CONNECT_IO_TYPE emIOType = CONNECT_IO_TCP);

//File_Message�Ĳ���
class _File_Message_Param
{
public:
    uint32 m_u4ConnectID;
    IFileTestManager* m_pFileTest;
    bool m_blDelete;
    char* m_pPacketDebugData;
    uint32 m_u4PacketDebugSize;
    ACE_INET_Addr m_addrRemote;

    _File_Message_Param()
    {
        m_u4ConnectID       = 0;
        m_pFileTest         = NULL;
        m_blDelete          = false;
        m_pPacketDebugData  = NULL;
        m_u4PacketDebugSize = 0;
    }
};

//�ļ�ѹ�������빤���߳�
bool Tcp_Common_File_Message(_File_Message_Param obj_File_Message_Param, IBuffPacket*& pBuffPacket, const char* pConnectName);

//Tcp_Common_ClientInfo�Ĳ���
class _ClientConnectInfo_Param
{
public:
    uint32 m_u4ConnectID;
    ACE_INET_Addr m_addrRemote;
    uint32 m_u4AllRecvCount;
    uint32 m_u4AllSendCount;
    uint32 m_u4AllRecvSize;
    uint32 m_u4AllSendSize;
    ACE_Time_Value m_atvConnect;
    uint32 m_u4RecvQueueCount;
    uint64 m_u8RecvQueueTimeCost;
    uint64 m_u8SendQueueTimeCost;

    _ClientConnectInfo_Param()
    {
        m_u4ConnectID         = 0;
        m_u4AllRecvCount      = 0;
        m_u4AllSendCount      = 0;
        m_u4AllRecvSize       = 0;
        m_u4AllSendSize       = 0;
        m_u4RecvQueueCount    = 0;
        m_u8RecvQueueTimeCost = 0;
        m_u8SendQueueTimeCost = 0;
    }
};

//���ص�ǰHandler����Ϣ
_ClientConnectInfo Tcp_Common_ClientInfo(_ClientConnectInfo_Param obj_ClientConnectInfo_Param);

//Tcp_Common_Send_Input_To_Cache�Ĳ���
class _Input_To_Cache_Param
{
public:
    uint32 m_u4ConnectID;
    uint32 m_u4PacketParseInfoID;
    uint32 m_u4SendMaxBuffSize;
    uint8 m_u1SendType;
    uint16 m_u2CommandID;
    bool m_blDelete;

    _Input_To_Cache_Param()
    {
        m_u4ConnectID         = 0;
        m_u4PacketParseInfoID = 0;
        m_u4SendMaxBuffSize   = 0;
        m_u1SendType          = 0;
        m_u2CommandID         = 0;
        m_blDelete            = false;
    }
};

//����������뷢�ͻ�����
bool Tcp_Common_Send_Input_To_Cache(_Input_To_Cache_Param obj_Input_To_Cache_Param,
                                    ACE_Message_Block* pBlockMessage, uint32& u4PacketSize,
                                    IBuffPacket*& pBuffPacket);

//Tcp_Common_Make_Send_Packet�Ĳ���
class _Send_Packet_Param
{
public:
    uint32 m_u4ConnectID;
    uint8  m_u1SendType;
    uint32 m_u4PacketParseInfoID;
    uint32 m_u4SendMaxBuffSize;
    uint16 m_u2CommandID;
    bool m_blDelete;

    _Send_Packet_Param()
    {
        m_u4ConnectID         = 0;
        m_u1SendType          = 0;
        m_u4PacketParseInfoID = 0;
        m_u4SendMaxBuffSize   = 0;
        m_u2CommandID         = 0;
        m_blDelete            = false;
    }
};

//��װ��������
bool Tcp_Common_Make_Send_Packet(_Send_Packet_Param obj_Send_Packet_Param,
                                 IBuffPacket*& pBuffPacket,
                                 ACE_Message_Block* pBlockMessage,
                                 ACE_Message_Block*& pMbData,
                                 uint32& u4PacketSize);

//����Manager��Ϣ���йر���Ϣ
bool Tcp_Common_CloseConnect_By_Queue(uint32 u4ConnectID, CSendMessagePool& objSendMessagePool,
                                      uint32 u4SendQueuePutTime, ACE_Task<ACE_MT_SYNCH>* pTask);

//Tcp_Common_Manager_Post_Message�Ĳ���
class _Post_Message_Param
{
public:
    uint32 m_u4ConnectID;
    uint8 m_u1SendType;
    uint16 m_u2CommandID;
    uint8 m_u1SendState;
    bool m_blDelete;
    int m_nMessageID;
    uint16 m_u2SendQueueMax;
    uint32 m_u4SendQueuePutTime;

    _Post_Message_Param()
    {
        m_u4ConnectID        = 0;
        m_u1SendType         = 0;
        m_u2CommandID        = 0;
        m_u1SendState        = 0;
        m_blDelete           = false;
        m_nMessageID         = 0;
        m_u2SendQueueMax     = 0;
        m_u4SendQueuePutTime = 0;
    }
};

//����Manager��Ϣ���з���������Ϣ
bool Tcp_Common_Manager_Post_Message(_Post_Message_Param obj_Post_Message_Param, IBuffPacket* pBuffPacket,
                                     CSendMessagePool& objSendMessagePool,
                                     ACE_Task<ACE_MT_SYNCH>* pTask);

//��ʱ�����ͳ����־
void Tcp_Common_Manager_Timeout_CheckInfo(int nActiveConnectCount);

//�����������
_ClientNameInfo Tcp_Common_ClientNameInfo(uint32 u4ConnectID, const char* pConnectName, const char* pClientIP, int nClientPort, bool IsLog);

//��ʼ��Manager
void  Tcp_Common_Manager_Init(uint16 u2Index, CCommandAccount& objCommandAccount, uint16& u2SendQueueMax, CSendCacheManager& objSendCacheManager);

#endif
