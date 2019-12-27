#include "ace/OS_NS_stdio.h"
#include "ace/OS_NS_string.h"
#include "ace/Thread_Manager.h"
#include "ace/ARGV.h"
#include "ace/Reactor.h"

#include "ace/TP_Reactor.h"
#include "ace/Dev_Poll_Reactor.h"
#include "Frame_Logging_Strategy.h"

static void run_reactor(ACE_Reactor* pReactor)
{
    pReactor->owner(ACE_Thread_Manager::instance ()->thr_self());
    pReactor->run_reactor_event_loop ();

    ACE_DEBUG((LM_INFO, ACE_TEXT("(%P|%t) %M run_reactor exit[%N,%l]\n")));
}


Logging_Config_Param::Logging_Config_Param()
{
    //��־�ļ���ȫ·��
    ACE_OS::snprintf(m_strLogFile, 256, "%s%s", m_strLogFile, "/serverdebug.log");
}

//��־����
Frame_Logging_Strategy::Frame_Logging_Strategy()
{
}

void Frame_Logging_Strategy::Close()
{
    ACE_DEBUG((LM_INFO, ACE_TEXT("[Frame_Logging_Strategy::Close]Begin\n")));

    pLogStrategy->fini();

    if (pLogStrategy != NULL)
    {
        pLogStrategy->reactor(NULL);
        delete pLogStrategy;
    }

    if (pLogStraReactor != NULL)
    {
        pLogStraReactor->close();
        delete pLogStraReactor;
    }

    ACE_DEBUG((LM_INFO, ACE_TEXT("[Frame_Logging_Strategy::Close]End\n")));
}

std::string Frame_Logging_Strategy::GetLogLevel(const std::string& strLogLevel)
{
    std::string strOutLogLevel="";

    if(strLogLevel == "DEBUG")
    {
        strOutLogLevel = "DEBUG|INFO|NOTICE|WARNING|ERROR|CRITICAL|ALERT|EMERGENCY";
    }
    else if(strLogLevel == "INFO")
    {
        strOutLogLevel = "~DEBUG|INFO|NOTICE|WARNING|ERROR|CRITICAL|ALERT|EMERGENCY";
    }
    else if(strLogLevel=="NOTICE")
    {
        strOutLogLevel = "~DEBUG|~INFO|NOTICE|WARNING|ERROR|CRITICAL|ALERT|EMERGENCY";
    }
    else if(strLogLevel=="WARNING")
    {
        strOutLogLevel = "~DEBUG|~INFO|~NOTICE|WARNING";
    }
    else if(strLogLevel=="ERROR")
    {
        strOutLogLevel = "~DEBUG|~INFO|~WARNING|~NOTICE|ERROR|CRITICAL|ALERT|EMERGENCY";
    }
    else if(strLogLevel=="CRITICAL")
    {
        strOutLogLevel = "~DEBUG|~INFO|~WARNING|~NOTICE|~ERROR|CRITICAL|ALERT|EMERGENCY";
    }
    else if(strLogLevel=="ALERT")
    {
        strOutLogLevel = "~DEBUG|~INFO|~WARNING|~NOTICE|~ERROR|~CRITICAL|ALERT|EMERGENCY";
    }
    else if(strLogLevel=="EMERGENCY")
    {
        strOutLogLevel = "~DEBUG|~INFO|~WARNING|~NOTICE|~ERROR|~CRITICAL|~ALERT|EMERGENCY";
    }
    else
    {
        strOutLogLevel = "DEBUG|INFO|WARNING|NOTICE|ERROR|CRITICAL|ALERT|EMERGENCY";
    }

    return strOutLogLevel;
}

//��ʼ����־����
int Frame_Logging_Strategy::InitLogStrategy(Logging_Config_Param& ConfigParam)
{
    //Set Arg List
    char cmdline[1024] = {0};

    std::string strTemp = ConfigParam.m_strLogLevel;
    std::string strLogLevel = GetLogLevel(strTemp);

    if(ConfigParam.m_bSendTerminal)
    {
        ACE_OS::snprintf(cmdline,
                         1024,
                         "-s %s -f STDERR -p %s -i %d -m %d -N %d",
                         ConfigParam.m_strLogFile,
                         strLogLevel.c_str(),
                         ConfigParam.m_iChkInterval,
                         ConfigParam.m_iLogFileMaxSize,
                         ConfigParam.m_iLogFileMaxCnt);
    }
    else
    {
        ACE_OS::snprintf(cmdline,
                         1024,
                         "-s %s -f OSTREAM -p %s -i %d -m %d -N %d",
                         ConfigParam.m_strLogFile,
                         strLogLevel.c_str(),
                         ConfigParam.m_iChkInterval,
                         ConfigParam.m_iLogFileMaxSize,
                         ConfigParam.m_iLogFileMaxCnt);
    }

    ACE_Reactor_Impl* pImpl = 0;

    ACE_NEW_RETURN (pImpl, ACE_TP_Reactor, -1);


    ACE_NEW_RETURN(pLogStraReactor, ACE_Reactor(pImpl,1), -1);
    ACE_NEW_RETURN(pLogStrategy, My_ACE_Logging_Strategy, -1);

    //Set Reactor
    pLogStrategy->reactor(pLogStraReactor);

    ACE_ARGV args;
    args.add(ACE_TEXT(cmdline));

    pLogStrategy->init(args.argc(),args.argv());

    if (ACE_Thread_Manager::instance ()->spawn(ACE_THR_FUNC (run_reactor), (void*)pLogStraReactor) == -1)
    {
        ACE_ERROR_RETURN ((LM_ERROR,"Spawning Reactor.\n"),-1);
    }

    ACE_DEBUG((LM_INFO, ACE_TEXT("(%P|%t) %M Init Log Strategy Success [%N,%l]\n")));
    return 0;
}

int Frame_Logging_Strategy::InitLogStrategy()
{
    Logging_Config_Param cfgParam;
    return InitLogStrategy(cfgParam);
}

//��������
int Frame_Logging_Strategy::EndLogStrategy()
{
    if(NULL != pLogStrategy)
    {
        ACE_DEBUG((LM_INFO, ACE_TEXT("[Frame_Logging_Strategy::EndLogStrategy]Begin\n")));
        pLogStraReactor->end_reactor_event_loop();
        ACE_DEBUG((LM_INFO, ACE_TEXT("[Frame_Logging_Strategy::EndLogStrategy]End\n")));
    }

    return 0;
}

My_ACE_Logging_Strategy* Frame_Logging_Strategy::GetStrategy()
{
    return pLogStrategy;
}

