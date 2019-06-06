#include "TcpRedirection.h"

CTcpRedirection::CTcpRedirection() : m_u2Count(0), m_pClientManager(NULL), m_pConnectManager(NULL)
{

}


void CTcpRedirection::Close()
{
    OUR_DEBUG((LM_INFO, "[CTcpRedirection::Close]Begin.\n"));

    if (m_u2Count > 0)
    {
        //ɾ��hash��ռ�
        m_objRedirectList.Close();

        m_u2Count = 0;
    }

    OUR_DEBUG((LM_INFO, "[CTcpRedirection::Close]End.\n"));
}

void CTcpRedirection::Init(std::vector<xmlTcpRedirection::_RedirectionInfo> vecInfo, uint32 u4MaxHandlerCount, IClientManager* pClientManager, IConnectManager* pConnectManager)
{
    //����Hash������������100
    m_objRedirectList.Init(MAX_BUFF_100);
    m_objRedirectConnectList.Init(u4MaxHandlerCount);
    m_pClientManager  = pClientManager;
    m_pConnectManager = pConnectManager;

    for (int i = 0; i < (int)vecInfo.size(); i++)
    {
        xmlTcpRedirection::_RedirectionInfo* pRedirectionInfo = &vecInfo[i];

        m_objRedirectList.Add_Hash_Data_By_Key_Unit32(pRedirectionInfo->SrcPort, pRedirectionInfo);
    }

    m_u2Count = (uint16)vecInfo.size();
}

void CTcpRedirection::ConnectRedirect(uint32 u4SrcPort, uint32 u4ConnectID)
{
    if (m_u2Count == 0 || NULL == m_pClientManager)
    {
        return;
    }

    xmlTcpRedirection::_RedirectionInfo* pRedirectionInfo = m_objRedirectList.Get_Hash_Box_Data_By_Uint32(u4SrcPort);

    if (NULL != pRedirectionInfo)
    {
        //����Զ�̷�����
        CRedirectionData* pRedirectionData = new CRedirectionData();
        pRedirectionData->SetServerID(u4ConnectID);
        pRedirectionData->SetConnectManager(m_pConnectManager);
        pRedirectionData->SetMode(pRedirectionInfo->Mode);
        pRedirectionData->SetConnectState(pRedirectionInfo->ConnectState);

        m_objRedirectConnectList.Add_Hash_Data_By_Key_Unit32(u4ConnectID, pRedirectionData);

        m_pClientManager->Connect(u4ConnectID,
                                  pRedirectionInfo->RedirectionIP.c_str(),
                                  pRedirectionInfo->RedirectionPort,
                                  TYPE_IPV4,
                                  (IClientMessage*)pRedirectionData);

        //����Ҫ����ͬ���ȴ��м���������ӽ����ɹ��ټ���
        int nRunCount = 0;

        while (true)
        {
            if (SERVER_CONNECT_FIRST == m_pClientManager->GetConnectState(u4ConnectID)
                || SERVER_CONNECT_OK == m_pClientManager->GetConnectState(u4ConnectID)
                || nRunCount >= MAX_CONNECT_REDIRECTION_COUNT)
            {
                break;
            }

            ACE_Time_Value tvSleep(0, 1000);
            ACE_OS::sleep(tvSleep);
            nRunCount++;
        }
    }
}

void CTcpRedirection::DataRedirect(uint32 u4ConnectID, ACE_Message_Block* mb)
{
    if (m_u2Count == 0 || NULL == m_pClientManager)
    {
        return;
    }

    CRedirectionData* pRedirectionData = m_objRedirectConnectList.Get_Hash_Box_Data_By_Uint32(u4ConnectID);

    if (NULL != pRedirectionData)
    {
        char* pData = (char*)mb->rd_ptr();

        //���������״̬���ŷ�������
        if (SERVER_CONNECT_FIRST == m_pClientManager->GetConnectState(u4ConnectID)
            || SERVER_CONNECT_OK == m_pClientManager->GetConnectState(u4ConnectID))
        {
            m_pClientManager->SendData(u4ConnectID, pData, (int)mb->length(), false);
        }
    }
}

void CTcpRedirection::CloseRedirect(uint32 u4ConnectID)
{
    if (m_u2Count == 0 || NULL == m_pClientManager)
    {
        return;
    }

    //�����ڴ�����
    CRedirectionData* pRedirectionData = m_objRedirectConnectList.Get_Hash_Box_Data_By_Uint32(u4ConnectID);

    if (NULL != pRedirectionData)
    {
        OUR_DEBUG((LM_INFO, "[CTcpRedirection::CloseRedirect]u4ConnectID=%d.\n", u4ConnectID));
        SAFE_DELETE(pRedirectionData);
        m_objRedirectConnectList.Del_Hash_Data_By_Unit32(u4ConnectID);
        m_pClientManager->Close(u4ConnectID);
    }
}

bool CTcpRedirection::GetMode(uint32 u4LocalPort)
{
    bool blRet = true;

    if (m_u2Count == 0 || NULL == m_pClientManager)
    {
        return blRet;
    }

    xmlTcpRedirection::_RedirectionInfo* pRedirectionInfo = m_objRedirectList.Get_Hash_Box_Data_By_Uint32(u4LocalPort);

    if (NULL != pRedirectionInfo && pRedirectionInfo->Mode == 1)
    {
        blRet = false;
    }

    return blRet;
}
