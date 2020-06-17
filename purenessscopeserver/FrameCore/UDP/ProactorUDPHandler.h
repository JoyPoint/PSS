#ifndef _PROACTORUDPHANDLE_H
#define _PROACTORUDPHANDLE_H

#include "ace/ACE.h"
#include "ace/OS_main.h"
#include "ace/OS_NS_string.h"
#include "ace/Proactor.h"
#include "ace/Asynch_IO.h"
#include "ace/INET_Addr.h"
#include "ace/SOCK_Dgram.h"

#include "XmlConfig.h"
#include "BaseHander.h"
#include "ForbiddenIP.h"
#include "CommandAccount.h"
#include "LoadPacketParse.h"

class CProactorUDPHandler : public ACE_Service_Handler
{
public:
    CProactorUDPHandler(void);
    ~CProactorUDPHandler(void);

    void SetPacketParseInfoID(uint32 u4PacketParseInfoID);                    //���ö�Ӧ��m_u4PacketParseInfoID
    virtual void handle_read_dgram(const ACE_Asynch_Read_Dgram::Result& result);

    int  OpenAddress(const ACE_INET_Addr& AddrLocal, ACE_Proactor* pProactor);
    void Close();
    bool SendMessage(char*& pMessage, uint32 u4Len, const char* szIP, uint16 u2Port, bool blHead = true, uint16 u2CommandID = 0, bool blDlete = true);
    _ClientConnectInfo GetClientConnectInfo();
    void GetCommandData(uint16 u2CommandID, _CommandData& objCommandData);    //���ָ������ͳ����Ϣ
    void GetFlowInfo(uint32& u4FlowIn, uint32& u4FlowOut);                    //�õ����е�������Ϣ
    void SetRecvSize(uint32 u4RecvSize);                                      //���ý������ݰ����ߴ�
    uint32 GetRecvSize();                                                     //�õ����ݰ����ߴ�

private:
    bool CheckMessage(ACE_Message_Block* pMbData, uint32 u4Len);              //����������ݰ����������ݶ���
    void SaveProSendInfo(uint32 u4Len);                                       //��¼������Ϣ

    CPacketParse*           m_pPacketParse;                 //���ݰ�������
    ACE_INET_Addr           m_addrRemote;                   //���ݷ��ͷ���IP��Ϣ
    ACE_INET_Addr           m_addrLocal;                    //��������IP��Ϣ
    ACE_SOCK_Dgram          m_skRemote;
    ACE_Asynch_Read_Dgram   m_Read;
    ACE_Asynch_Write_Dgram  m_Write;

    ACE_Time_Value          m_atvInput;                     //���հ���ʱ��
    ACE_Time_Value          m_atvOutput;                    //���Ͱ���ʱ��
    uint32                  m_u4RecvPacketCount;            //�������ݰ�������
    uint32                  m_u4SendPacketCount;            //�������ݰ�������
    uint32                  m_u4RecvSize;                   //�������ݵ��ܴ�С
    uint32                  m_u4SendSize;                   //�������ݵ��ܴ�С
    uint32                  m_u4PacketParseInfoID;          //��Ӧ����packetParse��ģ��ID
    uint32                  m_u4MaxRecvSize;                //���������ݰ��ߴ�
    char                    m_szCompletionkey[MAX_BUFF_20]; //��ɶ˿ڵ�Key
    char                    m_szAct[MAX_BUFF_20];           //����
    _TimeConnectInfo        m_TimeConnectInfo;              //���ӽ��������
    CCommandAccount         m_CommandAccount;               //���ݰ�ͳ��


};

#endif
