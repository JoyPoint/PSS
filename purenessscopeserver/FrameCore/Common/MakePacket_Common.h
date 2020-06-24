#ifndef MAKEPACKET_COMMON_H
#define MAKEPACKET_COMMON_H

#include "define.h"
#include "PacketParse.h"

class _MakePacket
{
public:
	uint32              m_u4ConnectID     = 0;                                   //����ID
	uint32              m_u4PacketParseID = 0;                                   //��ӦConnectID��PacketParseID
	int32               m_nHashID         = 0;                                   //��Ӧ��¼hash��ID
	CPacketParse* m_pPacketParse          = NULL;                                //���ݰ�����ָ��
	EM_CONNECT_IO_TYPE  m_u1PacketType    = EM_CONNECT_IO_TYPE::CONNECT_IO_TCP;  //���ݰ�����
	uint8               m_u1Option        = 0;                                   //��������
	ACE_INET_Addr       m_AddrRemote;                                            //���ݰ�����ԴIP��Ϣ
	ACE_INET_Addr       m_AddrListen;                                            //���ݰ���Դ����IP��Ϣ

	_MakePacket()
	{
	}

	void Clear()
	{
		m_u1PacketType = EM_CONNECT_IO_TYPE::CONNECT_IO_TCP;  //0ΪTCP,1��UDP Ĭ����TCP
		m_u4ConnectID = 0;
		m_u1Option = 0;
		m_pPacketParse = NULL;
		m_u4PacketParseID = 0;
	}

	void SetHashID(int32 nHashID)
	{
		m_nHashID = nHashID;
	}

	int32 GetHashID() const
	{
		return m_nHashID;
	}
};

#endif