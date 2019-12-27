
#pragma once

#include "MyACELoggingStrategy.h"
#include <string>

const int CONFIG_ARGV_COUNT = 6;

//�޸�ACE_Logging_Strategy��һ��BUG
//����ڶ��߳���˫д���µ�tellp()�����̰߳�ȫ������
//�����޸�ACEԴ�룬�������һ������

class Logging_Config_Param
{
public:
    Logging_Config_Param();

    //�ļ���С���ʱ��(Secs)
    int m_iChkInterval = 600;

    //ÿ����־�ļ�����С(KB)
    int m_iLogFileMaxSize = 10240;

    //��־�ļ�������
    int m_iLogFileMaxCnt = 3;

    //�Ƿ����ն˷���
    int m_bSendTerminal = 0;

    //��־�ȼ�����
    char m_strLogLevel[128] = {'\0'};

    //��־�ļ���ȫ·��
    char m_strLogFile[256] = { '\0' };
};

class Frame_Logging_Strategy
{
public:
    Frame_Logging_Strategy();

    void Close();

    //��־����
    std::string GetLogLevel(const std::string& strLogLevel);

    int InitLogStrategy();

    //��ʼ����־����
    int InitLogStrategy(Logging_Config_Param& ConfigParam);

    //��������
    int EndLogStrategy();

    //�õ�pLogStrategyָ��
    My_ACE_Logging_Strategy* GetStrategy();

private:
    ACE_Reactor* pLogStraReactor          = NULL;
    My_ACE_Logging_Strategy* pLogStrategy = NULL;
};

