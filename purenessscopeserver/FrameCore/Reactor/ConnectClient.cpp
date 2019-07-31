R#include "ConnectClient.h"
#include "ClientReConnectManager.h"

CConnectClient::CConnectClient(void)
{
    m_pCurrMessage      = NULL;
    m_nIOCount          = 1;
    m_nServerID         = 0;
    m_u4SendSize        = 0;
    m_u4SendCount       = 0;
    m_u4RecvSize        = 0;
    m_u4RecvCount       = 0;
    m_u4CostTime        = 0;
    m_u4MaxPacketSize   = MAX_MSG_PACKETLENGTH;
    m_ems2s             = S2S_NEED_CALLBACK;
    m_szError[0]        = '\0';
    m_pClientMessage    = NULL;
    m_u4CurrSize        = 0;
    m_u1ConnectState    = 0;

    m_emRecvState       = SERVER_RECV_INIT;
    m_emDispose         = CONNECT_IO_PLUGIN;
    m_u4PacketParseInfoID = 0;

}

CConnectClient::~CConnectClient(void)
{
}

void CConnectClient::Close()
{
    m_ThreadLock.acquire();

    if (m_nIOCount > 0)
    {
        m_nIOCount--;
    }

    m_ThreadLock.release();

    //�ӷ�Ӧ��ע���¼�
    if (m_nIOCount == 0)
    {
        shutdown();
        OUR_DEBUG((LM_ERROR, "[CConnectClient::Close]Close(%s:%d) OK.\n", m_addrRemote.get_host_addr(), m_addrRemote.get_port_number()));

        //ɾ�����Ӷ���
        App_ClientReConnectManager::instance()->CloseByClient(m_nServerID);

        if (CONNECT_IO_FRAME == m_emDispose)
        {
            //����packetParse�Ͽ���Ϣ
            _ClientIPInfo objClientIPInfo;
            _ClientIPInfo objLocalIPInfo;
            App_PacketParseLoader::instance()->GetPacketParseInfo(m_u4PacketParseInfoID)->Connect(m_nServerID,
                    objClientIPInfo,
                    objLocalIPInfo);

            //���Ϳ����Ϣ
            Send_MakePacket_Queue(m_nServerID,
                                  m_u4PacketParseInfoID,
                                  NULL,
                                  PACKET_SERVER_TCP_DISCONNECT,
                                  m_addrRemote,
                                  "127.0.0.1",
                                  0,
                                  CONNECT_IO_SERVER_TCP);
        }

        //�ع��ù���ָ��
        delete this;
    }

}

void CConnectClient::ClientClose()
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_ThreadLock);
    //��������Ϣ�Ͽ�����outputȥִ�У������Ͳ���Ҫͬ�������ˡ�
    ACE_Message_Block* pMbData = App_MessageBlockManager::instance()->Create(sizeof(int));

    if (NULL == pMbData)
    {
        OUR_DEBUG((LM_ERROR, "[CConnectClient::ClientClose] Connectid=%d, pMbData is NULL.\n", GetServerID()));
        return;
    }

    ACE_Message_Block::ACE_Message_Type objType = ACE_Message_Block::MB_STOP;
    pMbData->msg_type(objType);

    //���������رգ�����Ҫ��ص����ӶϿ���Ϣ
    SetClientMessage(NULL);

    //����Ϣ������У���output�ڷ�Ӧ���̷߳��͡�
    ACE_Time_Value xtime = ACE_OS::gettimeofday();

    //���������������ٷŽ�ȥ��,�Ͳ��Ž�ȥ��
    if (msg_queue()->is_full() == true)
    {
        OUR_DEBUG((LM_ERROR, "[CConnectClient::ClientClose] Connectid=%d,putq is full(%d).\n", GetServerID(), msg_queue()->message_count()));
        App_MessageBlockManager::instance()->Close(pMbData);
        return;
    }

    if (this->putq(pMbData, &xtime) == -1)
    {
        OUR_DEBUG((LM_ERROR, "[CConnectClient::ClientClose] Connectid=%d,putq(%d) output errno = [%d].\n", GetServerID(), msg_queue()->message_count(), errno));
        App_MessageBlockManager::instance()->Close(pMbData);
    }
    else
    {
        m_u1ConnectState = CONNECT_CLIENT_CLOSE;
        int nWakeupRet = reactor()->schedule_wakeup(this, ACE_Event_Handler::WRITE_MASK);

        if (-1 == nWakeupRet)
        {
            OUR_DEBUG((LM_ERROR, "CConnectClient::ClientClose] Connectid=%d, nWakeupRet(%d) output errno = [%d].\n", GetServerID(), nWakeupRet, errno));
        }
    }
}

int CConnectClient::open(void* p)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_ThreadLock);

    msg_queue()->high_water_mark(MAX_MSG_MASK);
    msg_queue()->low_water_mark(MAX_MSG_MASK);

    //�������ļ���ȡ����
    m_u4MaxPacketSize  = GetXmlConfigAttribute(xmlRecvInfo)->RecvBuffSize;

    if (p != NULL)
    {
        OUR_DEBUG((LM_ERROR, "[CConnectClient::open]p is not NULL.\n"));
    }

    ACE_Time_Value nowait(MAX_MSG_PACKETTIMEOUT);
    m_nIOCount = 1;

    //��������Ϊ������ģʽ
    if (this->peer().enable(ACE_NONBLOCK) == -1)
    {
        OUR_DEBUG((LM_ERROR, "[CConnectHandler::open]this->peer().enable  = ACE_NONBLOCK error.\n"));
        sprintf_safe(m_szError, MAX_BUFF_500, "[CConnectClient::open]this->peer().enable  = ACE_NONBLOCK error.");
        return -1;
    }

    //���Զ�����ӵ�ַ�Ͷ˿�
    if (this->peer().get_remote_addr(m_addrRemote) == -1)
    {
        OUR_DEBUG((LM_ERROR, "[CConnectHandler::open]this->peer().get_remote_addr error.\n"));
        sprintf_safe(m_szError, MAX_BUFF_500, "[CConnectClient::open]this->peer().get_remote_addr error.");
        return -1;
    }

    m_u4SendSize        = 0;
    m_u4SendCount       = 0;
    m_u4RecvSize        = 0;
    m_u4RecvCount       = 0;
    m_u4CostTime        = 0;
    m_atvBegin          = ACE_OS::gettimeofday();
    m_u4CurrSize        = 0;
    //���뵱ǰ��MessageBlock
    m_pCurrMessage = App_MessageBlockManager::instance()->Create(GetXmlConfigAttribute(xmlConnectServer)->Recvbuff);

    if (m_pCurrMessage == NULL)
    {
        OUR_DEBUG((LM_ERROR, "[CConnectClient::RecvClinetPacket] pmb new is NULL.\n"));
        return -1;
    }

    m_u1ConnectState = CONNECT_OPEN;

    OUR_DEBUG((LM_INFO, "[CConnectClient::open] Connection from [%s:%d]\n", m_addrRemote.get_host_addr(), m_addrRemote.get_port_number()));

    if (CONNECT_IO_FRAME == m_emDispose)
    {
        //����packetParse�Ͽ���Ϣ
        _ClientIPInfo objClientIPInfo;
        _ClientIPInfo objLocalIPInfo;
        App_PacketParseLoader::instance()->GetPacketParseInfo(m_u4PacketParseInfoID)->Connect(m_nServerID,
                objClientIPInfo,
                objLocalIPInfo);

        //���Ϳ����Ϣ
        Send_MakePacket_Queue(m_nServerID,
                              m_u4PacketParseInfoID,
                              NULL,
                              PACKET_SERVER_TCP_CONNECT,
                              m_addrRemote,
                              "127.0.0.1",
                              0,
                              CONNECT_IO_SERVER_TCP);
    }

    int nRet = this->reactor()->register_handler(this, ACE_Event_Handler::READ_MASK | ACE_Event_Handler::WRITE_MASK);

    int nWakeupRet = reactor()->cancel_wakeup(this, ACE_Event_Handler::WRITE_MASK);

    if (-1 == nWakeupRet)
    {
        OUR_DEBUG((LM_ERROR, "[CConnectClient::open]ConnectID=%d, nWakeupRet=%d, errno=%d.\n", GetServerID(), nWakeupRet, errno));
    }

    return nRet;
}

int CConnectClient::handle_input(ACE_HANDLE fd)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_ThreadLock);

    if (fd == ACE_INVALID_HANDLE)
    {
        OUR_DEBUG((LM_ERROR, "[CConnectClient::handle_input]fd == ACE_INVALID_HANDLE.\n"));
        sprintf_safe(m_szError, MAX_BUFF_500, "[CConnectHandler::handle_input]fd == ACE_INVALID_HANDLE.");

        if (NULL != m_pClientMessage)
        {
            _ClientIPInfo objServerIPInfo;
            sprintf_safe(objServerIPInfo.m_szClientIP, MAX_BUFF_20, "%s", m_addrRemote.get_host_addr());
            objServerIPInfo.m_nPort = m_addrRemote.get_port_number();

            if(S2S_NEED_CALLBACK == m_ems2s)
            {
                m_pClientMessage->ConnectError((int)ACE_OS::last_error(), objServerIPInfo);
            }
        }

        return -1;
    }

    //�жϻ����Ƿ�ΪNULL
    if (m_pCurrMessage == NULL)
    {
        m_u4CurrSize = 0;
        OUR_DEBUG((LM_ERROR, "[CConnectClient::handle_input]m_pCurrMessage == NULL.\n"));
        sprintf_safe(m_szError, MAX_BUFF_500, "[CConnectClient::handle_input]m_pCurrMessage == NULL.");

        if (NULL != m_pClientMessage)
        {
            _ClientIPInfo objServerIPInfo;
            sprintf_safe(objServerIPInfo.m_szClientIP, MAX_BUFF_20, "%s", m_addrRemote.get_host_addr());
            objServerIPInfo.m_nPort = m_addrRemote.get_port_number();

            if(S2S_NEED_CALLBACK == m_ems2s)
            {
                m_pClientMessage->ConnectError((int)ACE_OS::last_error(), objServerIPInfo);
            }
        }

        return -1;
    }

    return RecvData();
}

int CConnectClient::RecvData()
{
    ACE_Time_Value nowait(MAX_MSG_PACKETTIMEOUT);

    int nCurrCount = (uint32)m_pCurrMessage->size();

    int nDataLen = (int)this->peer().recv(m_pCurrMessage->wr_ptr(), nCurrCount, MSG_NOSIGNAL, &nowait);

    if (nDataLen <= 0)
    {
        m_u4CurrSize = 0;
        uint32 u4Error = (uint32)errno;
        OUR_DEBUG((LM_ERROR, "[CConnectClient::handle_input] ConnectID = %d, recv data is error nDataLen = [%d] errno = [%d].\n", GetServerID(), nDataLen, u4Error));
        sprintf_safe(m_szError, MAX_BUFF_500, "[CConnectClient::handle_input] ConnectID = %d, recv data is error[%d].\n", GetServerID(), nDataLen);

        _ClientIPInfo objServerIPInfo;
        sprintf_safe(objServerIPInfo.m_szClientIP, MAX_BUFF_20, "%s", m_addrRemote.get_host_addr());
        objServerIPInfo.m_nPort = m_addrRemote.get_port_number();

        if(S2S_NEED_CALLBACK == m_ems2s)
        {
            m_pClientMessage->ConnectError((int)ACE_OS::last_error(), objServerIPInfo);
        }

        return -1;
    }

    //�����DEBUG״̬����¼��ǰ���ܰ��Ķ���������
    Output_Debug_Data(m_pCurrMessage, LOG_SYSTEM_DEBUG_SERVERRECV);

    m_pCurrMessage->wr_ptr(nDataLen);

    Dispose_Recv_Data(m_pCurrMessage);

    m_emRecvState = SERVER_RECV_END;

    m_pCurrMessage->reset();

    return 0;
}

int CConnectClient::SendMessageGroup(uint16 u2CommandID, ACE_Message_Block* pmblk)
{
    //��֯����
    return Make_Common_Dispose_Client_WorkTread_Message(u2CommandID, (uint32)GetServerID(), pmblk, m_addrRemote);
}

int CConnectClient::Dispose_Recv_Data(ACE_Message_Block* pCurrMessage)
{
    if (CONNECT_IO_FRAME == m_emDispose)
    {
        _Packet_Parse_Info* pPacketParseInfo = App_PacketParseLoader::instance()->GetPacketParseInfo(m_u4PacketParseInfoID);

        if (NULL != pPacketParseInfo)
        {
            _Packet_Info obj_Packet_Info;
            uint8 n1Ret = pPacketParseInfo->Parse_Packet_Stream(m_nServerID,
                          pCurrMessage,
                          dynamic_cast<IMessageBlockManager*>(App_MessageBlockManager::instance()),
                          &obj_Packet_Info,
                          CONNECT_IO_SERVER_TCP);

            if (PACKET_GET_ENOUGH == n1Ret)
            {

                //������Ϣ����Ϣ���
                CPacketParse* pPacketParse = App_PacketParsePool::instance()->Create(__FILE__, __LINE__);
                pPacketParse->SetPacket_Head_Message(obj_Packet_Info.m_pmbHead);
                pPacketParse->SetPacket_Body_Message(obj_Packet_Info.m_pmbBody);
                pPacketParse->SetPacket_CommandID(obj_Packet_Info.m_u2PacketCommandID);
                pPacketParse->SetPacket_Head_Src_Length(obj_Packet_Info.m_u4HeadSrcLen);
                pPacketParse->SetPacket_Head_Curr_Length(obj_Packet_Info.m_u4HeadCurrLen);
                pPacketParse->SetPacket_Body_Src_Length(obj_Packet_Info.m_u4BodySrcLen);
                pPacketParse->SetPacket_Body_Curr_Length(obj_Packet_Info.m_u4BodyCurrLen);

                //���Ϳ����Ϣ
                Send_MakePacket_Queue(m_nServerID,
                                      m_u4PacketParseInfoID,
                                      pPacketParse,
                                      PACKET_PARSE,
                                      m_addrRemote,
                                      "127.0.0.1",
                                      0,
                                      CONNECT_IO_SERVER_TCP);

                //���������m_pPacketParse
                App_PacketParsePool::instance()->Delete(pPacketParse);
            }
            else if (PACKET_GET_ERROR == n1Ret)
            {
                // ���ݰ��������󣬶Ͽ�����
                Close();
                return 0;
            }
        }
    }
    else
    {
        //�������ݣ����ظ��߼��㣬�Լ������������������ж�
        _ClientIPInfo objServerIPInfo;
        sprintf_safe(objServerIPInfo.m_szClientIP, MAX_BUFF_20, "%s", m_addrRemote.get_host_addr());
        objServerIPInfo.m_nPort = m_addrRemote.get_port_number();

        uint16 u2CommandID = 0;
        ACE_Message_Block* pRecvFinish = NULL;

        m_atvRecv = ACE_OS::gettimeofday();
        m_emRecvState = SERVER_RECV_BEGIN;
        EM_PACKET_ROUTE em_PacketRoute = PACKET_ROUTE_SELF;

        while (true)
        {
            bool blRet = m_pClientMessage->Recv_Format_data(pCurrMessage, App_MessageBlockManager::instance(), u2CommandID, pRecvFinish, em_PacketRoute);

            if (false == blRet)
            {
                break;
            }
            else
            {
                if (PACKET_ROUTE_SELF == em_PacketRoute)
                {
                    //ֱ�ӵ��ò����ע��Ķ���������
                    Recv_Common_Dispose_Client_Message(u2CommandID, pRecvFinish, objServerIPInfo, m_pClientMessage);
                }
                else
                {
                    //�����ݷŻص���Ϣ�߳�
                    SendMessageGroup(u2CommandID, pRecvFinish);
                }
            }
        }
    }

    return 0;
}

void CConnectClient::Output_Debug_Data(ACE_Message_Block* pMbData, int nLogType, bool blLog)
{
    char szPacketDebugData[MAX_BUFF_1024] = { '\0' };

    if (GetXmlConfigAttribute(xmlServerType)->Debug == DEBUG_ON || false == blLog)
    {
        int nDataLen = (int)pMbData->length();
        char szLog[10] = { '\0' };
        uint32 u4DebugSize = 0;
        bool blblMore = false;

        if ((uint32)nDataLen >= MAX_BUFF_200)
        {
            u4DebugSize = MAX_BUFF_200 - 1;
            blblMore = true;
        }
        else
        {
            u4DebugSize = (uint32)nDataLen;
        }

        char* pData = pMbData->rd_ptr();

        for (uint32 i = 0; i < u4DebugSize; i++)
        {
            sprintf_safe(szLog, 10, "0x%02X ", (unsigned char)pData[i]);
            sprintf_safe(szPacketDebugData + 5 * i, MAX_BUFF_1024 - 5 * i, "%s", szLog);
        }

        szPacketDebugData[5 * u4DebugSize] = '\0';

        if (blblMore == true)
        {
            AppLogManager::instance()->WriteLog(nLogType, "[%s:%d]%s.(���ݰ�����ֻ��¼ǰ200�ֽ�)", m_addrRemote.get_host_addr(), m_addrRemote.get_port_number(), szPacketDebugData);
        }
        else
        {
            AppLogManager::instance()->WriteLog(nLogType, "[%s:%d]%s.", m_addrRemote.get_host_addr(), m_addrRemote.get_port_number(), szPacketDebugData);
        }
    }
}

int CConnectClient::handle_close(ACE_HANDLE h, ACE_Reactor_Mask mask)
{
    if (h == ACE_INVALID_HANDLE)
    {
        OUR_DEBUG((LM_DEBUG, "[CConnectClient::handle_close] h is NULL mask=%d.\n", GetServerID(), (int)mask));
    }

    Close();
    return 0;
}

int CConnectClient::handle_output(ACE_HANDLE fd /*= ACE_INVALID_HANDLE*/)
{
    if (fd == ACE_INVALID_HANDLE)
    {
        m_u4CurrSize = 0;
        OUR_DEBUG((LM_ERROR, "[CConnectClient::handle_output]fd == ACE_INVALID_HANDLE.\n"));
        return -1;
    }

    ACE_Message_Block* pmbSendData = NULL;
    ACE_Time_Value nowait(ACE_OS::gettimeofday());

    while (-1 != this->getq(pmbSendData, &nowait))
    {
        if (NULL == pmbSendData)
        {
            OUR_DEBUG((LM_INFO, "[CConnectClient::handle_output]ConnectID=%d pmbSendData is NULL.\n", GetServerID()));
            break;
        }

        //����ǶϿ�ָ���ִ�����ӶϿ�����
        if (pmbSendData->msg_type() == ACE_Message_Block::MB_STOP)
        {
            return -1;
        }

        //��������
        char* pData = pmbSendData->rd_ptr();

        if (NULL == pData)
        {
            OUR_DEBUG((LM_ERROR, "[CConnectClient::SendData] ConnectID = %d, pData is NULL.\n", GetServerID()));
            App_MessageBlockManager::instance()->Close(pmbSendData);
            return -1;
        }

        int nSendLen = (int)pmbSendData->length();   //�������ݵ��ܳ���
        int nIsSendSize = 0;

        //ѭ�����ͣ�ֱ�����ݷ�����ɡ�
        while (true)
        {
            if (nSendLen <= 0)
            {
                OUR_DEBUG((LM_ERROR, "[CConnectClient::SendData] ConnectID = %d, nCurrSendSize error is %d.\n", GetServerID(), nSendLen));
                App_MessageBlockManager::instance()->Close(pmbSendData);
                return -1;
            }

            int nDataLen = (int)this->peer().send(pmbSendData->rd_ptr(), nSendLen - nIsSendSize, &nowait);

            if (nDataLen <= 0)
            {
                _ClientIPInfo objServerIPInfo;
                sprintf_safe(objServerIPInfo.m_szClientIP, MAX_BUFF_20, "%s", m_addrRemote.get_host_addr());
                objServerIPInfo.m_nPort = m_addrRemote.get_port_number();
                m_pClientMessage->ConnectError((int)ACE_OS::last_error(), objServerIPInfo);

                OUR_DEBUG((LM_ERROR, "[CConnectClient::SendData] ConnectID = %d, error = %d.\n", GetServerID(), errno));
                App_MessageBlockManager::instance()->Close(pmbSendData);
                return -1;
            }
            else if (nDataLen + nIsSendSize >= nSendLen)  //�����ݰ�ȫ��������ϣ���ա�
            {
                App_MessageBlockManager::instance()->Close(pmbSendData);
                m_u4SendSize += (uint32)nSendLen;
                m_u4SendCount++;
                break;
            }
            else
            {
                pmbSendData->rd_ptr(nDataLen);
                nIsSendSize += nDataLen;
            }
        }

    }

    int nWakeupRet = reactor()->cancel_wakeup(this, ACE_Event_Handler::WRITE_MASK);

    if (-1 == nWakeupRet)
    {
        OUR_DEBUG((LM_INFO, "[CConnectClient::handle_output]ConnectID=%d,nWakeupRet=%d, errno=%d.\n", GetServerID(), nWakeupRet, errno));
    }

    return 0;
}

void CConnectClient::SetClientMessage(IClientMessage* pClientMessage)
{
    m_pClientMessage = pClientMessage;
}

void CConnectClient::SetServerID(int nServerID)
{
    m_nServerID = nServerID;
}

int CConnectClient::GetServerID()
{
    return m_nServerID;
}

void CConnectClient::SetPacketParseInfoID(uint32 u4PacketParseInfoID)
{
    m_u4PacketParseInfoID = u4PacketParseInfoID;
}

bool CConnectClient::SendData(ACE_Message_Block* pmblk)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_ThreadLock);

    if (CONNECT_CLIENT_CLOSE == m_u1ConnectState || CONNECT_SERVER_CLOSE == m_u1ConnectState)
    {
        //�����Ѿ�����ر����̣����ڽ��ܷ������ݡ�
        App_MessageBlockManager::instance()->Close(pmblk);
        return false;
    }

    if (NULL == pmblk)
    {
        OUR_DEBUG((LM_ERROR, "[CConnectClient::SendData] ConnectID = %d, get_handle() == ACE_INVALID_HANDLE.\n", GetServerID()));
        return false;
    }

    //�����DEBUG״̬����¼��ǰ���ܰ��Ķ���������
    Output_Debug_Data(pmblk, LOG_SYSTEM_DEBUG_CLIENTSEND);

    ACE_Time_Value     nowait(MAX_MSG_PACKETTIMEOUT);

    if (get_handle() == ACE_INVALID_HANDLE)
    {
        OUR_DEBUG((LM_ERROR, "[CConnectClient::SendData] ConnectID = %d, get_handle() == ACE_INVALID_HANDLE.\n", GetServerID()));
        sprintf_safe(m_szError, MAX_BUFF_500, "[CConnectClient::SendData] ConnectID = %d, get_handle() == ACE_INVALID_HANDLE.\n", GetServerID());
        App_MessageBlockManager::instance()->Close(pmblk);
        return false;
    }

    //����Ϣ������У���output�ڷ�Ӧ���̷߳��͡�
    ACE_Time_Value xtime = ACE_OS::gettimeofday();

    //���������������ٷŽ�ȥ��,�Ͳ��Ž�ȥ��
    if (msg_queue()->is_full() == true)
    {
        OUR_DEBUG((LM_ERROR, "[CConnectClient::SendMessage] Connectid=%d,putq is full(%d).\n", GetServerID(), msg_queue()->message_count()));
        App_MessageBlockManager::instance()->Close(pmblk);
        return false;
    }

    if (this->putq(pmblk, &xtime) == -1)
    {
        OUR_DEBUG((LM_ERROR, "[CConnectClient::SendMessage] Connectid=%d,putq(%d) output errno = [%d].\n", GetServerID(), msg_queue()->message_count(), errno));
        App_MessageBlockManager::instance()->Close(pmblk);
    }
    else
    {
        int nWakeupRet = reactor()->schedule_wakeup(this, ACE_Event_Handler::WRITE_MASK);

        if (-1 == nWakeupRet)
        {
            OUR_DEBUG((LM_ERROR, "[CConnectClient::SendMessage] Connectid=%d, nWakeupRet(%d) output errno = [%d].\n", GetServerID(), nWakeupRet, errno));
        }
    }

    return true;
}

_ClientConnectInfo CConnectClient::GetClientConnectInfo()
{
    _ClientConnectInfo ClientConnectInfo;
    ClientConnectInfo.m_blValid       = true;
    ClientConnectInfo.m_addrRemote    = m_addrRemote;
    ClientConnectInfo.m_u4AliveTime   = (uint32)(ACE_OS::gettimeofday().sec() - m_atvBegin.sec());
    ClientConnectInfo.m_u4AllRecvSize = m_u4RecvSize;
    ClientConnectInfo.m_u4RecvCount   = m_u4RecvCount;
    ClientConnectInfo.m_u4AllSendSize = m_u4SendSize;
    ClientConnectInfo.m_u4SendCount   = m_u4SendCount;
    ClientConnectInfo.m_u4ConnectID   = m_nServerID;
    ClientConnectInfo.m_u4BeginTime   = (uint32)m_atvBegin.sec();
    return ClientConnectInfo;
}

bool CConnectClient::GetTimeout(ACE_Time_Value const& tvNow)
{
    ACE_Time_Value tvIntval(tvNow - m_atvRecv);

    if(m_emRecvState == SERVER_RECV_BEGIN && tvIntval.sec() > SERVER_RECV_TIMEOUT)
    {
        //�������ݴ����Ѿ���ʱ���������ӡ����
        OUR_DEBUG((LM_DEBUG,"[CConnectClient::GetTimeout]***(%d)recv dispose is timeout(%d)!***.\n", m_nServerID, tvIntval.sec()));
        return false;
    }
    else
    {
        return true;
    }
}


