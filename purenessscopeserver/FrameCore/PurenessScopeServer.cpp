// PurenessScopeServer.cpp : �������̨Ӧ�ó������ڵ㡣
//
// ��ʱ���˵������վ�������Բ���һ�����׵����飬���������ڱ���վ������Ҫ�������ģ�freeeyes
// add by freeeyes, freebird92
// 2008-12-22(����)
// ��Twitter����������������һЩ���õ�С���ɣ������ںϡ�
// û��Ŀ��ļ�������õģ�Ŭ������PSS�����ʺϿ������������ٿ����߿���������д���������ʡ�
// ���ṩ���걸�Ĵ���������ƣ���һЩ���õĳ����̼��ɡ�
// ����PSS����һ������Ŭ���ˣ������˸���Ļ�飬���ǻ�����㲻�ϳɳ���
// add by freeeyes
// 2013-09-24

#include "Frame_Logging_Strategy.h"

#ifndef WIN32
//�����Linux
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "WaitQuitSignal.h"
#include "ServerManager.h"

#include "ace/Thread.h"
#include "ace/Synch.h"
#else
//�����windows
#include "WindowsProcess.h"
#include "WindowsDump.h"
#include <windows.h>
#endif

//�������������ļ��е�PacketParseģ��
int Load_PacketParse_Module()
{
    int nPacketParseCount = (int)GetXmlConfigAttribute(xmlPacketParses)->vec.size();
    App_PacketParseLoader::instance()->Init(nPacketParseCount);

    for (uint8 i = 0; i < nPacketParseCount; i++)
    {
        bool blState = App_PacketParseLoader::instance()->LoadPacketInfo(GetXmlConfigAttribute(xmlPacketParses)->vec[i].ParseID,
                       GetXmlConfigAttribute(xmlPacketParses)->vec[i].Type,
                       GetXmlConfigAttribute(xmlPacketParses)->vec[i].OrgLength,
                       GetXmlConfigAttribute(xmlPacketParses)->vec[i].ModulePath.c_str(),
                       GetXmlConfigAttribute(xmlPacketParses)->vec[i].ModuleName.c_str());

        if (false == blState)
        {
            //������ʽ����PacketParse
            App_PacketParseLoader::instance()->Close();
            return -1;
        }
    }

    return 0;
}

#ifndef WIN32
//�ر���Ϣ������������
ACE_Thread_Mutex g_mutex;
ACE_Condition<ACE_Thread_Mutex> g_cond(g_mutex);


//����ź����߳�
void* thread_Monitor(void* arg)
{
    if(NULL != arg)
    {
        OUR_DEBUG((LM_INFO, "[thread_Monitor]arg is not NULL.\n"));
        pthread_exit(0);
    }

    g_mutex.acquire();

    bool blFlag = true;

    while(WaitQuitSignal::wait(blFlag))
    {
        sleep(1);
    }

    g_cond.signal();
    g_mutex.release();

    OUR_DEBUG((LM_INFO, "[thread_Monitor]exit.\n"));
    pthread_exit(0);
}

int CheckCoreLimit(int nMaxCoreFile)
{
    //��õ�ǰCore��С����
    struct rlimit rCorelimit;

    if(getrlimit(RLIMIT_CORE, &rCorelimit) != 0)
    {
        OUR_DEBUG((LM_INFO, "[CheckCoreLimit]failed to getrlimit number of files.\n"));
        return -1;
    }

    if(nMaxCoreFile != 0)
    {
        OUR_DEBUG((LM_INFO, "[CheckCoreLimit]** WARNING!WARNING!WARNING!WARNING! **.\n"));
        OUR_DEBUG((LM_INFO, "[CheckCoreLimit]** PSS WILL AUTO UP CORE SIZE LIMIT **.\n"));
        OUR_DEBUG((LM_INFO, "[CheckCoreLimit]** WARNING!WARNING!WARNING!WARNING! **.\n"));
        rCorelimit.rlim_cur = RLIM_INFINITY;
        rCorelimit.rlim_max = RLIM_INFINITY;

        if (setrlimit(RLIMIT_CORE, &rCorelimit)!= 0)
        {
            OUR_DEBUG((LM_INFO, "[CheckCoreLimit]failed to setrlimit core size(error=%s).\n", strerror(errno)));
            return -1;
        }
    }
    else
    {
        if((int)rCorelimit.rlim_cur > 0)
        {
            //����ҪCore�ļ��ߴ磬�������Core�ļ���С���ó�0
            rCorelimit.rlim_cur = (rlim_t)nMaxCoreFile;
            rCorelimit.rlim_max = (rlim_t)nMaxCoreFile;

            if (setrlimit(RLIMIT_CORE, &rCorelimit)!= 0)
            {
                OUR_DEBUG((LM_INFO, "[Checkfilelimit]failed to setrlimit number of files.\n"));
                return -1;
            }
        }
    }

    return 0;
}

//���õ�ǰ����·��
bool SetAppPath()
{
    int nSize = (int)pathconf(".",_PC_PATH_MAX);

    if (nSize <= 0)
    {
        OUR_DEBUG((LM_INFO, "[SetAppPath]pathconf is error(%d).\n", nSize));
        return false;
    }

    char* pFilePath = new char[nSize];

    char szPath[MAX_BUFF_300] = { '\0' };
    memset(pFilePath, 0, nSize);
    ACE_OS::snprintf(pFilePath, MAX_BUFF_300, "/proc/%d/exe", getpid());

    //�ӷ��������л�õ�ǰ�ļ�ȫ·�����ļ���
    ssize_t stPathSize = readlink(pFilePath, szPath, MAX_BUFF_300 - 1);

    if (stPathSize <= 0)
    {
        OUR_DEBUG((LM_INFO, "[SetAppPath]no find work Path.\n", szPath));
        SAFE_DELETE_ARRAY(pFilePath);
        return false;
    }
    else
    {
        SAFE_DELETE_ARRAY(pFilePath);
    }

    while(szPath[stPathSize - 1]!='/')
    {
        stPathSize--;
    }

    szPath[stPathSize > 0 ? (stPathSize-1) : 0]= '\0';

    int nRet = chdir(szPath);

    if (-1 == nRet)
    {
        OUR_DEBUG((LM_INFO, "[SetAppPath]Set work Path (%s) fail.\n", szPath));
    }
    else
    {
        OUR_DEBUG((LM_INFO, "[SetAppPath]Set work Path (%s) OK.\n", szPath));
    }

    return true;
}

//�ӽ��̳���
int Chlid_Run()
{
    //�ж��Ƿ�����Ҫ�Է����״̬����
    if(GetXmlConfigAttribute(xmlServerType)->Type == 1)
    {
        OUR_DEBUG((LM_INFO, "[main]Procress is run background.\n"));
        Gdaemon();
    }

    //��ʽ����PacketParse
    if (0 != Load_PacketParse_Module())
    {
        pthread_exit(NULL);
    }

    //�жϵ�ǰCore�ļ��ߴ��Ƿ���Ҫ����
    if(-1 == CheckCoreLimit(GetXmlConfigAttribute(xmlCoreSetting)->CoreNeed))
    {
        return 0;
    }

    //���ü���ź������߳�
    WaitQuitSignal::init();

    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_create(&tid, &attr, thread_Monitor, NULL);

    //�ȴ��߳�����Ч
    ACE_Time_Value tvSleep(0, 1000);
    ACE_OS::sleep(tvSleep);

    //�ڶ��������������������
    if(!App_ServerManager::instance()->Init())
    {
        OUR_DEBUG((LM_INFO, "[main]App_ServerManager::instance()->Init() error.\n"));
        App_ServerManager::instance()->Close();
        App_PacketParseLoader::instance()->Close();
        return 0;
    }

    OUR_DEBUG((LM_INFO, "[CServerManager::Start]Begin.\n"));

    if(!App_ServerManager::instance()->Start())
    {
        OUR_DEBUG((LM_INFO, "[main]App_ServerManager::instance()->Start() error.\n"));
        App_ServerManager::instance()->Close();
        App_PacketParseLoader::instance()->Close();
        return 0;
    }

    OUR_DEBUG((LM_INFO, "[main]Server Run is End.\n"));

    g_mutex.acquire();

    OUR_DEBUG((LM_INFO, "[main]Server Exit.\n"));

    //������ʽ����PacketParse
    App_PacketParseLoader::instance()->Close();

    return 0;
}

int ACE_TMAIN(int argc, ACE_TCHAR* argv[])
{
    if(argc > 0)
    {
        OUR_DEBUG((LM_INFO, "[main]argc = %d.\n", argc));

        for(int i = 0; i < argc; i++)
        {
            OUR_DEBUG((LM_INFO, "[main]argc(%d) = %s.\n", argc, argv[i]));
        }
    }

    //�������õ�ǰ����·��
    if (false == SetAppPath())
    {
        OUR_DEBUG((LM_INFO, "[main]SetAppPath error.\n"));
    }

    //��ȡ�����ļ�
    if(!App_XmlConfig::instance()->InitIsOk())
    {
        OUR_DEBUG((LM_INFO, "[main]App_XmlConfig::instance()->InitIsOk() is false.\n"));
        return 0;
    }

    if (0 != Chlid_Run())
    {
        OUR_DEBUG((LM_INFO, "[main]Chlid_Run error.\n"));
    }

    return 0;
}

#else

bool ctrlhandler(DWORD fdwctrltype)
{
    switch (fdwctrltype)
    {
    // handle the ctrl-c signal.
    case CTRL_C_EVENT:
        printf("ctrl-c event\n\n");
        return(true);

    // ctrl-close: confirm that the user wants to exit.
    case CTRL_CLOSE_EVENT:
        printf("ctrl-close event\n\n");
        App_ProServerManager::instance()->Close();
        return(true);

    // pass other signals to the next handler.
    case CTRL_BREAK_EVENT:
        printf("ctrl-break event\n\n");
        return false;

    case CTRL_LOGOFF_EVENT:
        printf("ctrl-logoff event\n\n");
        return false;

    case CTRL_SHUTDOWN_EVENT:
        printf("ctrl-shutdown event\n\n");
        App_ProServerManager::instance()->Close();
        return false;

    default:
        return false;
    }
}

int ACE_TMAIN(int argc, ACE_TCHAR* argv[])
{
    //ָ����ǰĿ¼����ֹ�����ļ�ʧ��
    TCHAR szFileName[MAX_PATH] = {0};
    GetModuleFileName(0, szFileName, MAX_PATH);
    LPTSTR pszEnd = _tcsrchr(szFileName, TEXT('\\'));

    if (pszEnd != 0)
    {
        pszEnd++;
        *pszEnd = 0;
    }

    OUR_DEBUG((LM_INFO, "[main]PSS is Starting.\n"));

    SetCurrentDirectory(szFileName);

    //���Dump�ļ�
    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)exception_handler);
    base_set_crash_handler(main_crash_handler, nullptr);

    //��Ӷ�Console  ����windowsϵͳ�Ļ��Ƶ���ctrlhandler��ʱ�޷�����ִ�н����ͱ�ϵͳ������������ʱ�ر�windows�ϵĻص�
#ifndef WIN32
    SetConsoleCtrlHandler((PHANDLER_ROUTINE)ctrlhandler, true);
#endif

    //��һ������ȡ�����ļ�
    if(!App_XmlConfig::instance()->InitIsOk())
    {
        OUR_DEBUG((LM_INFO, "[main]!App_XmlConfig->InitIsOk() is false.\n"));
        return 0;
    }

    //��ʽ����PacketParse
    if (0 != Load_PacketParse_Module())
    {
        return 0;
    }

    //�ж��Ƿ�����Ҫ�Է����״̬����
    if(GetXmlConfigAttribute(xmlServerType)->Type == 1)
    {
        App_Process::instance()->startprocesslog();

        //�Է���״̬����
        //���ȿ���û����������windows����
        App_Process::instance()->run(argc, argv);
    }
    else
    {
        //��������
        ServerMain();
    }

    //������ʽ����PacketParse
    App_PacketParseLoader::instance()->Close();

    if(GetXmlConfigAttribute(xmlServerType)->Type == 1)
    {
        App_Process::instance()->stopprocesslog();
    }

    return 0;
}

#endif


