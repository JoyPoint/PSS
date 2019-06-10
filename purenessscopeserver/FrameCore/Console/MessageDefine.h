#ifndef _MESSAGEDEFINE_H
#define _MESSAGEDEFINE_H

//���ﴦ�����еĽ������������
//��Щ������Ծ�̬�����ķ�ʽ��¶����
//���������ڴ�����Ķ�����Ϣ��ӳ��
//��ô����ԭ���ǣ����������if else ��������
//�Ż�����ṹ������sonar��������
//add by freeeyes

#include <ace/OS_NS_sys_resource.h>

#include "define.h"
#include "ForbiddenIP.h"

//�������ֵ���Ͷ���
enum
{
    CONSOLE_MESSAGE_SUCCESS = 0,
    CONSOLE_MESSAGE_FAIL    = 1,
    CONSOLE_MESSAGE_CLOSE   = 2,
};

static const char* COMMAND_SPLIT_STRING = " ";

//��������
class _CommandInfo
{
public:
    uint8 m_u1OutputType;                 //������ͣ�0Ϊ�����ƣ�1Ϊ�ı�
    char m_szCommandTitle[MAX_BUFF_100];  //��������ͷ
    char m_szCommandExp[MAX_BUFF_100];    //����������չ����
    char m_szUser[MAX_BUFF_50];           //�û���Ϣ

    _CommandInfo()
    {
        m_u1OutputType = 0;
        m_szCommandTitle[0] = '\0';
        m_szCommandExp[0] = '\0';
        m_szUser[0] = '\0';
    }
};

//�ļ����ṹ
class _FileInfo
{
public:
    char m_szFilePath[MAX_BUFF_100];
    char m_szFileName[MAX_BUFF_100];
    char m_szFileParam[MAX_BUFF_200];

    _FileInfo()
    {
        m_szFilePath[0] = '\0';
        m_szFileName[0] = '\0';
        m_szFileParam[0] = '\0';
    }
};

//�����˿���Ϣ
class _ListenInfo
{
public:
    uint32 m_u4Port;
    uint32 m_u4PacketParseID;
    uint8  m_u1IPType;
    char   m_szListenIP[MAX_BUFF_20];

    _ListenInfo()
    {
        m_szListenIP[0] = '\0';
        m_u4Port = 0;
        m_u1IPType = TYPE_IPV4;
        m_u4PacketParseID = 0;
    }
};

//ȾɫIP��Ϣ
class _DyeIPInfo
{
public:
    char   m_szClientIP[MAX_BUFF_20];   //Ⱦɫ�ͻ���IP
    uint16 m_u2MaxCount;                //�������
};

//Ⱦɫ��CommandID
class _DyeCommandInfo
{
public:
    uint16 m_u2CommandID;               //Ⱦɫ�ͻ�������
    uint16 m_u2MaxCount;                //�������
};

class _PoolName
{
public:
    char   m_szPoolName[MAX_BUFF_50];   //�ڴ������
    bool   m_blState;                   //��ǰ�ڴ�ش�����Ϣ״̬
};

//Э�����, �������ݲ���
bool GetCommandParam(const char* pCommand, const char* pTag, char* pData, int nMaxSize);                  //������ָ����Command����
bool GetFileInfo(const char* pFile, _FileInfo& FileInfo);                                                 //��һ��ȫ·���зֳ��ļ���
bool GetForbiddenIP(const char* pCommand, _ForbiddenIP& ForbiddenIP);                                     //�õ���ֹ��IP�б�
bool GetConnectServerID(const char* pCommand, int& nServerID);                                            //�õ�һ��ָ���ķ�����ID
bool GetDebug(const char* pCommand, uint8& u1Debug);                                                      //�õ���ǰ���õ�BUDEG
bool GetTrackIP(const char* pCommand, _ForbiddenIP& ForbiddenIP);                                         //�õ����õ�׷��IP
bool GetLogLevel(const char* pCommand, int& nLogLevel);                                                   //�õ���־�ȼ�
bool GetAIInfo(const char* pCommand, int& nAI, int& nDispose, int& nCheck, int& nStop);                   //�õ�AI����
bool GetNickName(const char* pCommand, char* pName);                                                      //�õ����ӱ���
bool GetConnectID(const char* pCommand, uint32& u4ConnectID, bool& blFlag);                               //�õ�ConnectID
bool GetMaxConnectCount(const char* pCommand, uint16& u2MaxConnectCount);                                 //�õ�������������
bool GetListenInfo(const char* pCommand, _ListenInfo& objListenInfo);                                     //�õ������˿���Ϣ
bool GetTestFileName(const char* pCommand, char* pFileName);                                              //��ü��ز����ļ���
bool GetDyeingIP(const char* pCommand, _DyeIPInfo& objDyeIPInfo);                                         //���ȾɫIP�������Ϣ
bool GetDyeingCommand(const char* pCommand, _DyeCommandInfo& objDyeCommandInfo);                          //���ȾɫCommand�������Ϣ
bool GetPoolSet(const char* pCommand, _PoolName& objPoolName);                                            //����ڴ�ص�����

#endif
