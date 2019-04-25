#include "IPAccount.h"


_IPAccount::_IPAccount()
{
    m_nCount = 0;
    m_nAllCount = 0;
    m_dtLastTime.update();
    m_nMinute = (int32)m_dtLastTime.minute();
}

void _IPAccount::Add(ACE_Date_Time dtNowTime)
{
    m_dtLastTime = dtNowTime;

    if (m_dtLastTime.minute() != m_nMinute)
    {
        m_nMinute = (int32)m_dtLastTime.minute();
        m_nCount = 1;
        m_nAllCount++;
    }
    else
    {
        m_nCount++;
        m_nAllCount++;
    }
}

bool _IPAccount::Check(ACE_Date_Time dtNowTime)
{
    //���3������û�и��£�������֮
    uint16 u2NowTime = (uint32)dtNowTime.minute();

    if (u2NowTime - m_nMinute < 0)
    {
        u2NowTime += 60;
    }

    if (u2NowTime - m_nMinute >= 3)
    {
        return false;
    }
    else
    {
        return true;
    }
}

//*********************************************

CIPAccount::CIPAccount()
{
    m_u4MaxConnectCount = 100;  //Ĭ��ÿ�����100��
    m_u4CurrConnectCount = 0;
    m_u4LastConnectCount = 0;
    m_u2CurrTime = 0;

    ACE_Date_Time  dtNowTime;
    m_u1Minute = (uint8)dtNowTime.minute();
}

CIPAccount::CIPAccount(const CIPAccount& ar)
{
    (*this) = ar;
}

CIPAccount::~CIPAccount()
{
    OUR_DEBUG((LM_INFO, "[CIPAccount::~CIPAccount].\n"));
    Close();
    OUR_DEBUG((LM_INFO, "[CIPAccount::~CIPAccount]End.\n"));
}

void CIPAccount::Close()
{
    for (int32 i = 0; i < m_objIPList.Get_Count(); i++)
    {
        _IPAccount* pIPAccount = m_objIPList.Pop();
        SAFE_DELETE(pIPAccount);
    }

    m_objIPList.Close();
}

void CIPAccount::Init(uint32 u4IPCount)
{
    m_u4MaxConnectCount = u4IPCount;

    //��ʼ��HashTable
    m_objIPList.Init((int32)u4IPCount);

    ACE_Date_Time  dtNowTime;
    m_u2CurrTime = (uint16)dtNowTime.minute();
}

bool CIPAccount::AddIP(string strIP)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> WGuard(m_ThreadLock);

    //��鵱ǰʱ�䣬10��������һ�ֵ�ǰHash����
    ACE_Date_Time  dtNowTime;
    uint16 u2NowTime = (uint16)dtNowTime.minute();

    Clear_Hash_Data(u2NowTime, dtNowTime);

    bool blRet = false;

    //������Ҫ����Ҫ�ж��������Ҫ�������IPͳ��
    if (m_u4MaxConnectCount > 0)
    {
        _IPAccount* pIPAccount = m_objIPList.Get_Hash_Box_Data(strIP.c_str());

        if (NULL == pIPAccount)
        {
            //û���ҵ�������
            pIPAccount = new _IPAccount();

            pIPAccount->m_strIP = strIP;
            pIPAccount->Add(dtNowTime);

            //�鿴�����Ƿ�����
            if (m_objIPList.Get_Count() == m_objIPList.Get_Used_Count()
                || -1 == m_objIPList.Add_Hash_Data(strIP.c_str(), pIPAccount))
            {
                //��ʱ������
                OUR_DEBUG((LM_INFO, "[CIPAccount::AddIP]Add_Hash_Data(%s) is error.\n", strIP.c_str()));
                SAFE_DELETE(pIPAccount);
                blRet = false;
            }
            else
            {
                blRet = true;
            }
        }
        else
        {
            pIPAccount->Add(dtNowTime);

            if ((uint32)pIPAccount->m_nCount >= m_u4MaxConnectCount)
            {
                blRet = false;
            }
            else
            {
                blRet = true;
            }
        }

    }
    else
    {
        blRet = true;
    }

    return blRet;
}

int32 CIPAccount::GetCount()
{
    return m_objIPList.Get_Used_Count();
}

uint32 CIPAccount::GetLastConnectCount()
{
    ACE_Date_Time  dtNowTime;
    uint16 u2NowTime = (uint16)dtNowTime.minute();

    //��鵱ǰʱ����������
    if (m_u1Minute != (uint8)u2NowTime)
    {
        m_u4LastConnectCount = m_u4CurrConnectCount;
        m_u4CurrConnectCount = 0;
        m_u1Minute = (uint8)u2NowTime;
    }

    return m_u4LastConnectCount;
}

void CIPAccount::GetInfo(vecIPAccount& VecIPAccount)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> WGuard(m_ThreadLock);
    vector<_IPAccount* > vecIPAccount;
    m_objIPList.Get_All_Used(vecIPAccount);

    for (int32 i = 0; i < (int32)vecIPAccount.size(); i++)
    {
        _IPAccount* pIPAccount = vecIPAccount[i];

        if (NULL != pIPAccount)
        {
            VecIPAccount.push_back(*pIPAccount);
        }
    }
}

void CIPAccount::Clear_Hash_Data(uint16 u2NowTime, ACE_Date_Time& dtNowTime)
{
    //��鵱ǰʱ����������
    if (m_u1Minute == (uint8)u2NowTime)
    {
        m_u4CurrConnectCount++;
    }
    else
    {
        m_u4LastConnectCount = m_u4CurrConnectCount;
        m_u4CurrConnectCount = 1;
        m_u1Minute = (uint8)u2NowTime;
    }

    if ((int32)(u2NowTime - m_u2CurrTime) < 0)
    {
        u2NowTime += 60;
    }

    if (u2NowTime - m_u2CurrTime >= 10)
    {
        //����Hash����
        vector<_IPAccount* > vecIPAccount;
        m_objIPList.Get_All_Used(vecIPAccount);

        for (int32 i = 0; i < (int32)vecIPAccount.size(); i++)
        {
            _IPAccount* pIPAccount = vecIPAccount[i];

            if (false == pIPAccount->Check(dtNowTime))
            {
                if (-1 == m_objIPList.Del_Hash_Data(pIPAccount->m_strIP.c_str()))
                {
                    OUR_DEBUG((LM_INFO, "[CIPAccount::AddIP]Del_Hash_Data(%s) is error.\n", pIPAccount->m_strIP.c_str()));
                }

                SAFE_DELETE(pIPAccount);
            }
        }
    }
}

//*********************************************************

CConnectAccount::CConnectAccount()
{
    m_u4ConnectMin = 0;
    m_u4ConnectMax = 0;
    m_u4DisConnectMin = 0;
    m_u4DisConnectMax = 0;
    m_u4CurrConnect = 0;
    m_u4CurrDisConnect = 0;
    m_u1Minute = 0;
}

CConnectAccount::~CConnectAccount()
{
}

uint32 CConnectAccount::Get4ConnectMin()
{
    return m_u4ConnectMin;
}

uint32 CConnectAccount::GetConnectMax()
{
    return m_u4ConnectMax;
}

uint32 CConnectAccount::GetDisConnectMin()
{
    return m_u4DisConnectMin;
}

uint32 CConnectAccount::GetDisConnectMax()
{
    return m_u4DisConnectMax;
}

uint32 CConnectAccount::GetCurrConnect()
{
    return m_u4CurrConnect;
}

uint32 CConnectAccount::GetCurrDisConnect()
{
    return m_u4CurrDisConnect;
}

void CConnectAccount::Init(uint32 u4ConnectMin, uint32 u4ConnectMax, uint32 u4DisConnectMin, uint32 u4DisConnectMax)
{
    m_u4ConnectMin = u4ConnectMin;
    m_u4ConnectMax = u4ConnectMax;
    m_u4DisConnectMin = u4DisConnectMin;
    m_u4DisConnectMax = u4DisConnectMax;
    m_u4CurrConnect = 0;
    m_u4CurrDisConnect = 0;

    ACE_Date_Time  dtLastTime;
    m_u1Minute = (uint8)dtLastTime.minute();
}

bool CConnectAccount::AddConnect()
{
    if (m_u4ConnectMax > 0)
    {
        ACE_Date_Time  dtLastTime;

        if (m_u1Minute != (uint8)dtLastTime.minute())
        {
            //�µ�һ����
            m_u4CurrConnect = 1;
        }
        else
        {
            m_u4CurrConnect++;
        }
    }

    return true;
}

bool CConnectAccount::AddDisConnect()
{
    if (m_u4ConnectMax > 0)
    {
        ACE_Date_Time  dtLastTime;

        if (m_u1Minute != (uint8)dtLastTime.minute())
        {
            //�µ�һ����
            m_u4CurrDisConnect = 1;
        }
        else
        {
            m_u4CurrDisConnect++;
        }
    }

    return true;
}

int32 CConnectAccount::CheckConnectCount()
{
    if (m_u4ConnectMax > 0 && m_u4CurrConnect > m_u4ConnectMax)
    {
        return 1;   //1Ϊ��Խmax����
    }

    if (m_u4ConnectMin > 0 && m_u4CurrConnect < m_u4ConnectMin)
    {
        return 2;    //2Ϊ����min����
    }

    return 0;
}

int32 CConnectAccount::CheckDisConnectCount()
{
    if (m_u4DisConnectMax > 0 && m_u4CurrDisConnect > m_u4DisConnectMax)
    {
        return 1;    //1Ϊ��Խmax����
    }

    if (m_u4ConnectMin > 0 && m_u4CurrDisConnect < m_u4DisConnectMin)
    {
        return 2;    //2Ϊ����min����
    }

    return 0;
}