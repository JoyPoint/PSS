#include "ProactorUDPHandler.h"

CProactorUDPHandler::CProactorUDPHandler(void)
{
    m_pPacketParse        = NULL;
    m_u4RecvPacketCount   = 0;
    m_u4SendPacketCount   = 0;
    m_u4RecvSize          = 0;
    m_u4SendSize          = 0;
    m_szCompletionkey[0]  = '\0';
    m_szAct[0]            = '\0';
    m_u4PacketParseInfoID = 0;
    m_u4MaxRecvSize       = MAX_BUFF_1024;
}

CProactorUDPHandler::~CProactorUDPHandler(void)
{
}

void CProactorUDPHandler::SetPacketParseInfoID(uint32 u4PacketParseInfoID)
{
    m_u4PacketParseInfoID = u4PacketParseInfoID;
}

int CProactorUDPHandler::OpenAddress(const ACE_INET_Addr& AddrLocal, ACE_Proactor* pProactor)
{
    if(m_skRemote.open(AddrLocal) == -1)
    {
        OUR_DEBUG((LM_ERROR, "[CProactorUDPHandler::OpenAddress]Open error(%d).\n", errno));
        return -1;
    }

    m_addrLocal = AddrLocal;

    sprintf_safe(m_szCompletionkey, MAX_BUFF_20, "CompUDP");
    sprintf_safe(m_szAct, MAX_BUFF_20, "ActUDP");

    //�����̳߳�ʼ��ͳ��ģ�������
    char szName[MAX_BUFF_50] = {'\0'};
    sprintf_safe(szName, MAX_BUFF_50, "�����߳�");
    m_CommandAccount.InitName(szName, GetXmlConfigAttribute(xmlCommandAccount)->MaxCommandCount);

    //��ʼ��ͳ��ģ�鹦��
    m_CommandAccount.Init(GetXmlConfigAttribute(xmlCommandAccount)->Account,
                          GetXmlConfigAttribute(xmlCommandAccount)->FlowAccount,
                          GetXmlConfigAttribute(xmlThreadInfo)->DisposeTimeout);


    //���÷��ͳ�ʱʱ�䣨��ΪUDP����ͻ��˲����ڵĻ���sendto������һ��recv����
    //����������һ����ʱ���ø�recv�������޵���ȥ
    struct timeval timeout = {MAX_RECV_UDP_TIMEOUT, 0};
    ACE_OS::setsockopt(m_skRemote.get_handle(), SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

    //Linux���޴˶�Ӧ�꣬Ϊ�˿���ͨ�ã��ⲿ��ֻ��windows������
#if PSS_PLATFORM == PLATFORM_WIN
    //����wsaIoctl
    bool blBehavior = false;
    unsigned long lRet = 0;
    int nStatus = ACE_OS::ioctl(m_skRemote.get_handle(), SIO_UDP_CONNRESET, &blBehavior, sizeof(blBehavior), NULL, 0, &lRet, NULL, NULL);

    if(0 != nStatus)
    {
        OUR_DEBUG((LM_ERROR, "[CProactorUDPHandler::OpenAddress]ioctl SIO_UDP_CONNRESET error.\n"));
    }

#endif

    if(m_Read.open(*this, m_skRemote.get_handle(), m_szCompletionkey, pProactor) == -1)
    {
        OUR_DEBUG((LM_ERROR, "[CProactorUDPHandler::OpenAddress]m_Read error.\n"));
        return -1;
    }

    if(m_Write.open(*this, m_skRemote.get_handle(), m_szCompletionkey, pProactor) == -1)
    {
        OUR_DEBUG((LM_ERROR, "[CProactorUDPHandler::OpenAddress]m_Write error.\n"));
        return -1;
    }

    ACE_Message_Block* pMBBuff = App_MessageBlockManager::instance()->Create(m_u4MaxRecvSize);

    if(NULL == pMBBuff)
    {
        OUR_DEBUG((LM_ERROR, "[CProactorUDPHandler::OpenAddress]pMBBuff is NULL.\n"));
        return -1;
    }

    //��ʼ�������
    m_TimeConnectInfo.Init(GetXmlConfigAttribute(xmlClientData)->RecvPacketCount,
                           GetXmlConfigAttribute(xmlClientData)->RecvDataMax,
                           GetXmlConfigAttribute(xmlClientData)->SendPacketCount,
                           GetXmlConfigAttribute(xmlClientData)->SendDataMax);

    m_pPacketParse = App_PacketParsePool::instance()->Create(__FILE__, __LINE__);

    size_t stRecvLen = m_u4MaxRecvSize;
    int nRecvSize = (int)m_Read.recv(pMBBuff, stRecvLen, 0, PF_INET, m_szAct);
    return nRecvSize;
}

void CProactorUDPHandler::Close()
{
    App_PacketParsePool::instance()->Delete(m_pPacketParse);
    m_Read.cancel();
    m_Write.cancel();
    m_skRemote.close();
}

void CProactorUDPHandler::handle_read_dgram(const ACE_Asynch_Read_Dgram::Result& result)
{
    //��ΪUDP��һ���Է������������ݰ������Բ�����ճ������
    ACE_Message_Block* pMBBuff = NULL;

    ACE_Message_Block* pMb = result.message_block();
    int nTran = (int)result.bytes_transferred();

    ACE_INET_Addr ClientAddr;
    result.remote_address(ClientAddr);
    m_addrRemote.set(ClientAddr);

    if(nTran != 0)
    {
        //��������
        if (false == CheckMessage(pMb, (uint32)nTran))
        {
            OUR_DEBUG((LM_INFO, "[CProactorUDPHandler::handle_read_dgram]CheckMessage error.\n"));
        }

        m_pPacketParse = App_PacketParsePool::instance()->Create(__FILE__, __LINE__);
    }
    else
    {
        OUR_DEBUG((LM_INFO, "[CProactorUDPHandler::handle_read_dgram]result.bytes_transferred() is 0.\n"));
    }

    App_MessageBlockManager::instance()->Close(pMb);
    pMBBuff = App_MessageBlockManager::instance()->Create(m_u4MaxRecvSize);

    if(NULL == pMBBuff)
    {
        OUR_DEBUG((LM_INFO, "[CProactorUDPHandler::handle_read_dgram]pMBBuff is NULL.\n"));
    }
    else
    {
        size_t stRecvLen = m_u4MaxRecvSize;
        m_Read.recv(pMBBuff, stRecvLen, 0, PF_INET, m_szAct);
    }
}

bool CProactorUDPHandler::SendMessage(char*& pMessage, uint32 u4Len, const char* szIP, int nPort, bool blHead, uint16 u2CommandID, bool blDlete)
{
    ACE_Message_Block* pMbData = NULL;
    ACE_INET_Addr AddrRemote;

    _Send_Message_Param obj_Send_Message_Param;
    obj_Send_Message_Param.m_u4PacketParseInfoID = m_u4PacketParseInfoID;
    obj_Send_Message_Param.m_blDlete             = blDlete;
    obj_Send_Message_Param.m_blHead              = blHead;
    obj_Send_Message_Param.m_nPort               = nPort;
    obj_Send_Message_Param.m_pIP                 = (char* )szIP;
    obj_Send_Message_Param.m_u2CommandID         = u2CommandID;
    obj_Send_Message_Param.m_u4Len               = u4Len;

    bool blState = Udp_Common_Send_Message(obj_Send_Message_Param,
                                           AddrRemote,
                                           pMessage,
                                           pMbData,
                                           m_skRemote);

    if (false == blState)
    {
        //�ͷŷ�����
        App_MessageBlockManager::instance()->Close(pMbData);

        return false;
    }

    SaveProSendInfo(u4Len);

    //ͳ�Ʒ�����Ϣ
    m_CommandAccount.SaveCommandData(u2CommandID, (uint32)nPort, CONNECT_IO_UDP, u4Len, COMMAND_TYPE_OUT);

    //�ͷŷ�����
    App_MessageBlockManager::instance()->Close(pMbData);

    return true;
}

_ClientConnectInfo CProactorUDPHandler::GetClientConnectInfo()
{
    _ClientConnectInfo ClientConnectInfo;
    ClientConnectInfo.m_blValid       = true;
    ClientConnectInfo.m_u4ConnectID   = 0;
    ClientConnectInfo.m_u4AliveTime   = 0;
    ClientConnectInfo.m_u4BeginTime   = (uint32)m_atvInput.sec();
    ClientConnectInfo.m_u4AllRecvSize = m_u4RecvSize;
    ClientConnectInfo.m_u4AllSendSize = m_u4SendSize;
    ClientConnectInfo.m_u4RecvCount   = m_u4RecvPacketCount;
    ClientConnectInfo.m_u4SendCount   = m_u4SendPacketCount;
    return ClientConnectInfo;
}

bool CProactorUDPHandler::CheckMessage(ACE_Message_Block* pMbData, uint32 u4Len)
{
    ACE_Time_Value tvCheck = ACE_OS::gettimeofday();

    if(NULL == m_pPacketParse || NULL == pMbData)
    {
        return false;
    }

    if(App_PacketParseLoader::instance()->GetPacketParseInfo(m_u4PacketParseInfoID)->m_u1PacketParseType == PACKET_WITHHEAD)
    {
        m_pPacketParse->SetPacket_Head_Src_Length(App_PacketParseLoader::instance()->GetPacketParseInfo(m_u4PacketParseInfoID)->m_u4OrgLength);

        if(u4Len < m_pPacketParse->GetPacketHeadSrcLen())
        {
            return false;
        }

        //�����������ݰ�ת��ΪPacketParse����
        ACE_Message_Block* pMBHead = App_MessageBlockManager::instance()->Create(m_pPacketParse->GetPacketHeadSrcLen());
        memcpy_safe((char* )pMbData->rd_ptr(), m_pPacketParse->GetPacketHeadSrcLen(), (char* )pMBHead->wr_ptr(), m_pPacketParse->GetPacketHeadSrcLen());
        pMBHead->wr_ptr(m_pPacketParse->GetPacketHeadSrcLen());

        bool blRet = Udp_Common_Recv_Head(pMBHead, m_pPacketParse, m_u4PacketParseInfoID, u4Len);

        if (false == blRet)
        {
            App_MessageBlockManager::instance()->Close(pMBHead);
            App_PacketParsePool::instance()->Delete(m_pPacketParse);
            return false;
        }

        //��ָ������ƶ�
        pMbData->rd_ptr(m_pPacketParse->GetPacketHeadSrcLen());

        //�����������
        if(m_pPacketParse->GetPacketBodySrcLen() > 0)
        {
            ACE_Message_Block* pMBBody = App_MessageBlockManager::instance()->Create(m_pPacketParse->GetPacketBodySrcLen());
            memcpy_safe((char* )pMbData->rd_ptr(), m_pPacketParse->GetPacketBodySrcLen(), (char* )pMBBody->wr_ptr(), m_pPacketParse->GetPacketBodySrcLen());
            pMBBody->wr_ptr(m_pPacketParse->GetPacketBodySrcLen());

            bool blStateBody = Udp_Common_Recv_Body(pMBBody, m_pPacketParse, m_u4PacketParseInfoID);

            if(false  == blStateBody)
            {
                App_MessageBlockManager::instance()->Close(pMBHead);
                App_MessageBlockManager::instance()->Close(pMBBody);
                App_PacketParsePool::instance()->Delete(m_pPacketParse);
                return false;
            }

        }

        //�������ݰ�
        if (false == Udp_Common_Send_WorkThread(m_pPacketParse, m_addrRemote, m_addrLocal, tvCheck))
        {
            return false;
        }
    }
    else
    {
        //������������
        if (false == Udp_Common_Recv_Stream(pMbData, m_pPacketParse, m_u4PacketParseInfoID))
        {
            App_PacketParsePool::instance()->Delete(m_pPacketParse);
            return false;
        }

        //�������ݰ�
        if (false == Udp_Common_Send_WorkThread(m_pPacketParse, m_addrRemote, m_addrLocal, tvCheck))
        {
            return false;
        }
    }

    App_PacketParsePool::instance()->Delete(m_pPacketParse);

    m_atvInput = ACE_OS::gettimeofday();
    m_u4RecvSize += u4Len;
    m_u4RecvPacketCount++;

    return true;
}

void CProactorUDPHandler::SaveProSendInfo(uint32 u4Len)
{
    m_atvOutput = ACE_OS::gettimeofday();
    m_u4SendSize += u4Len;
    m_u4SendPacketCount++;
}

void CProactorUDPHandler::GetCommandData(uint16 u2CommandID, _CommandData& objCommandData)
{
    _CommandData* pCommandData = m_CommandAccount.GetCommandData(u2CommandID);

    if(pCommandData != NULL)
    {
        objCommandData += (*pCommandData);
    }
}

void CProactorUDPHandler::GetFlowInfo(uint32& u4FlowIn, uint32& u4FlowOut)
{
    u4FlowIn  = m_CommandAccount.GetFlowIn();
    u4FlowOut = m_CommandAccount.GetFlowOut();
}

void CProactorUDPHandler::SetRecvSize(uint32 u4MaxRecvSize)
{
    m_u4MaxRecvSize = u4MaxRecvSize;
}

uint32 CProactorUDPHandler::GetRecvSize()
{
    return m_u4MaxRecvSize;
}
