#include "MessageDyeingManager.h"

CMessageDyeingManager::CMessageDyeingManager()
{
}

CMessageDyeingManager::~CMessageDyeingManager()
{

}

void CMessageDyeingManager::AddDyringIP(const char* pClientIP, uint16 u2MaxCount)
{
    m_objDyeingIP.m_u2MaxCount = u2MaxCount;
    m_objDyeingIP.m_u2CurrCount = 0;
    sprintf_safe(m_objDyeingIP.m_szIP, MAX_BUFF_20, "%s", pClientIP);
    m_objDyeingIP.m_blIsValid = true;
}

bool CMessageDyeingManager::AddDyeingCommand(uint16 u2CommandID, uint16 u2MaxCount)
{
    if (m_u2CurrCommandCount > MAX_DYEING_COMMAND_COUNT)
    {
        return false;
    }

    for (_Dyeing_Command& dyeing_command : m_objCommandList)
    {
        if (dyeing_command.m_blIsValid == false)
        {
            dyeing_command.m_u2CommandID = u2CommandID;
            dyeing_command.m_u2CurrCount = 0;
            dyeing_command.m_u2MaxCount  = u2MaxCount;
            dyeing_command.m_blIsValid = true;
            m_u2CurrCommandCount++;
            return true;
        }
    }

    return false;
}

void CMessageDyeingManager::GetDyeingCommand(vec_Dyeing_Command_list& objList)
{
    for (_Dyeing_Command& dyeing_command : m_objCommandList)
    {
        if (dyeing_command.m_blIsValid == true)
        {
            objList.push_back(dyeing_command);
        }
    }
}

string CMessageDyeingManager::GetTraceID(const char* pClientIP, short sClintPort, uint16 u2CommandID)
{
    string strTraceID = "";

    //����IP����
    if (true == m_objDyeingIP.m_blIsValid && ACE_OS::strcmp(m_objDyeingIP.m_szIP, pClientIP) == 0)
    {
        m_objDyeingIP.m_u2CurrCount++;

        //�ж��Ƿ�ﵽ��������
        if (m_objDyeingIP.m_u2CurrCount >= m_objDyeingIP.m_u2MaxCount)
        {
            m_objDyeingIP.Clear();
        }

        strTraceID = CREATE_TRACE(pClientIP, sClintPort, u2CommandID);
        return strTraceID;
    }

    //����������
    if(m_u2CurrCommandCount > 0)
    {
        for (_Dyeing_Command& dyeing_command : m_objCommandList)
        {
            if (false == dyeing_command.m_blIsValid || dyeing_command.m_u2CommandID != u2CommandID)
            {
                continue;
            }

            dyeing_command.m_u2CurrCount++;

            //�ж��Ƿ�ﵽ��������
            if (dyeing_command.m_u2CurrCount >= dyeing_command.m_u2MaxCount)
            {
                dyeing_command.Clear();
                m_u2CurrCommandCount--;
            }

            strTraceID = CREATE_TRACE(pClientIP, sClintPort, u2CommandID);
            return strTraceID;
        }
    }

    return strTraceID;
}

