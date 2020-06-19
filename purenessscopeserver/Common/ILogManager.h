#ifndef _ILOGMANAGER_H
#define _ILOGMANAGER_H

#include "ILogObject.h"

class ILogManager
{
public:
    virtual ~ILogManager() {}
    //�ı���־��¼
    template <class... Args>
    int WriteLog(int nLogType, const char* fmt, Args&& ... args)
    {
        char szBuff[MAX_BUFF_1024] = { '\0' };
        ACE_OS::snprintf(szBuff, MAX_BUFF_1024, fmt, convert(std::forward<Args>(args))...);
        uint32 u4Size = (uint32)ACE_OS::strlen(szBuff);
        return WriteLog_r(nLogType, szBuff, u4Size);
    };

    //�����ʼ���¼
    template <class... Args>
    int WriteToMail(int nLogType, uint16 u2MailID, const char* pTitle, const char* fmt, Args&& ... args)
    {
        char szBuff[MAX_BUFF_1024] = { '\0' };
        ACE_OS::snprintf(szBuff, MAX_BUFF_1024, fmt, convert(std::forward<Args>(args))...);
        uint32 u4Size = (uint32)ACE_OS::strlen(szBuff);
        return WriteToMail_r(nLogType, u4MailID, pTitle, szBuff, u4Size);
    };

    //��������־��¼
    virtual int WriteLogBinary(int nLogType, const char* pData, int nLen) = 0;

    //д����־�ӿ�
    virtual int WriteLog_r(int nLogType, const char* fmt, uint32 u4Len) = 0;

    //д���ʼ��ӿ�
    virtual int WriteToMail_r(int nLogType, uint16 u2MailID, const char* pTitle, const char* fmt, uint32 u4Len) = 0;
};

#endif
