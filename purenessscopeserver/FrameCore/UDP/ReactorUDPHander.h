#ifndef _REAACTORUDPHANDLER_H
#define _REAACTORUDPHANDLER_H

#include "ace/ACE.h"
#include "ace/OS_main.h"
#include "ace/OS_NS_string.h"
#include "ace/Event_Handler.h"
#include "ace/INET_Addr.h"
#include "ace/Reactor.h"
#include "ace/SOCK_Dgram.h"

#include "BaseHander.h"
#include "CommandAccount.h"

class CReactorUDPHander : public ACE_Event_Handler
{
public:
    CReactorUDPHander(void);
    ~CReactorUDPHander(void);

    virtual ACE_HANDLE get_handle(void) const;
    virtual int handle_input(ACE_HANDLE fd = ACE_INVALID_HANDLE);
    virtual int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask);

    void SetPacketParseInfoID(uint32 u4PacketParseInfoID);                    //���ö�Ӧ��m_u4PacketParseInfoID
    int  OpenAddress(const ACE_INET_Addr& AddrRemote, ACE_Reactor* pReactor);

    int  Run_Open(ACE_Reactor* pReactor);
    void Close();
    bool SendMessage(char*& pMessage, uint32 u4Len, const char* szIP, uint16 u2Port, bool blHead = true, uint16 u2CommandID = 0, bool blDlete = true);
    _ClientConnectInfo GetClientConnectInfo();
    void GetCommandData(uint16 u2CommandID, _CommandData& objCommandData);    //���ָ������ͳ����Ϣ
    void GetFlowInfo(uint32& u4FlowIn, uint32& u4FlowOut);                    //�õ����еĳ�������
    void SetRecvSize(uint32 u4RecvSize);                                      //���ý������ݰ����ߴ�
    uint32 GetRecvSize();                                                     //�õ����ݰ����ߴ�

private:
    bool CheckMessage(const char* pData, uint32 u4Len);              //����������ݰ����������ݶ���
    int  Init_Open_Address(const ACE_INET_Addr& AddrRemote);         //��ʼ��UDP���Ӷ���
    void SaveSendInfo(uint32 u4Len);                                 //��¼������Ϣ

    ACE_SOCK_Dgram          m_skRemote;
    ACE_INET_Addr           m_addrRemote;                           //���ݷ��ͷ���IP��Ϣ
    ACE_INET_Addr           m_addrLocal;                            //��������IP��Ϣ
    CPacketParse*           m_pPacketParse = NULL;                  //���ݰ�������
    char*                   m_pRecvBuff    = NULL;                  //�������ݻ���ָ��

    ACE_Time_Value          m_atvInput;                             //���հ���ʱ��
    ACE_Time_Value          m_atvOutput;                            //���Ͱ���ʱ��
    uint32                  m_u4RecvPacketCount   = 0;              //�������ݰ�������
    uint32                  m_u4SendPacketCount   = 0;              //�������ݰ�������
    uint32                  m_u4RecvSize          = 0;              //�������ݵ��ܴ�С
    uint32                  m_u4SendSize          = 0;              //�������ݵ��ܴ�С
    uint32                  m_u4MaxRecvSize       = MAX_BUFF_1024;  //���������ݰ��ߴ�
    uint32                  m_u4PacketParseInfoID = 0;              //��Ӧ����packetParse��ģ��ID
    CCommandAccount         m_CommandAccount;                       //���ݰ�ͳ��
};
#endif
