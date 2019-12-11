#ifndef _FILELOGGER_H
#define _FILELOGGER_H

#include "ace/FILE_Addr.h"
#include "ace/FILE_Connector.h"
#include "ace/FILE_IO.h"
#include "ace/Date_Time.h"

#include "define.h"
#include "ILogObject.h"
#include "smtps.h"
#include "XmlConfig.h"

const uint16 MAX_CMD_NUM = 100;
const uint16 MAX_TIME_SIZE = 100;

//����Log��������
static const char LOGTYPE_OPERATION[] = "Operation";
static const char LOGTYPE_ERROR[] = "ServerError";
static const char FILELOG_CONFIG[] = "logger.xml";

//��XML��ȡ����־�ļ���Ϣ
class _Log_File_Info
{
public:
    uint16 m_u2LogID;
    uint8  m_u1FileClass;
    uint8  m_u1DisPlay;
    uint16 m_u2LogLevel;
    char   m_szFileName[MAX_BUFF_100];

    _Log_File_Info()
    {
        m_u2LogID       = 0;
        m_u1FileClass   = 0;
        m_u1DisPlay     = 0;
        m_u2LogLevel    = 0;
        m_szFileName[0] = '\0';
    }
};

//��Ԫģʽ��־��
class CLogFile
{
public:
    CLogFile(const char* pFileRoot, uint32 u4BufferSize, uint32 u4FileMaxSize);

    virtual ~CLogFile() = default;

    void Init();

    void Close();

    void SetFileRoot(const char* pFileRoot);

    char* GetFileRoot();

    void SetFileAddr(ACE_FILE_Addr& objFileAddr);

    ACE_FILE_Addr& GetFileAddr();

    void SetConnector(ACE_FILE_Connector& objConnector);

    ACE_FILE_Connector& GetConnector();

    void SetFileIO(ACE_FILE_IO& objFile);

    ACE_FILE_IO& GetFileIO();

    void SetLogTime(const char* pLogTime);

    char* GetLogTime();

    void SetBufferSize(uint32 u4BufferSize);

    void SetFileMaxSize(uint32 u4FileMaxSize);

    void SetCurrFileIndex(uint16 u2CurrFileIndex);

    void SetCurrFileSize(uint32 u4CirrFileSize);

    uint32 GetBufferSize();

    uint32 GetFileMaxSize();

    uint16 GetCurrFileIndex();

    uint32 GetCurrFileSize();

    virtual int doLog(_LogBlockInfo* pLogBlockInfo);

    bool SendMail(_LogBlockInfo* pLogBlockInfo, xmlMails::_Mail* pMailInfo = NULL);

    ACE_TString& GetLoggerName();

    ACE_TString& GetServerName();

    int GetDisPlay();

    void SetDisplay(int nDisplay);

    void SetLoggerClass(int nType);

    int GetLoggerClass();

    void SetLoggerName(const char* szLoggerName);

    void SetLoggerID(uint16 u2LogID);

    uint16 GetLoggerID();

    void SetLevel(uint16 u2Level);

    uint16 GetLevel();

    void SetServerName(const char* szServerName);

    bool Run();

    void CheckTime();

    void CheckLogFileBlock();

    void CreatePath();

private:
    uint32              m_u4BufferSize;               //��־��������С
    uint32              m_u4CurrFileSize;             //��¼��ǰ�ļ���С
    uint32              m_u4FileMaxSize;              //������־�ļ�������С
    uint16              m_u2CurrFileIndex;            //��ǰ��־�����
    uint16              m_u2LogID;                    //��־���
    uint16              m_u2Level;                    //��־�ȼ�
    int                 m_nDisplay;                   //��ʾ���Ǽ�¼�ļ�
    char*               m_pBuffer;                    //��־����ָ��
    char                m_szLogTime[MAX_TIME_SIZE];   //Log��ǰʱ��
    char                m_szFileRoot[MAX_BUFF_100];   //·������Ŀ¼
    ACE_TString         m_StrlogName;                 //ģ������
    ACE_TString         m_StrlogType;                 //��־����
    ACE_TString         m_StrServerName;              //������ǰ׺
    ACE_FILE_Connector  m_Connector;                  //I/O����������
    ACE_FILE_IO         m_File;
    ACE_FILE_Addr       m_FileAddr;
};

class CFileLogger : public IServerLogger
{
public:
    CFileLogger();

    virtual int DoLog(int nLogType, _LogBlockInfo* pLogBlockInfo);
    virtual int GetLogTypeCount();

    bool Init();
    virtual bool ReSet(uint32 u4CurrLogLevel);
    virtual void Close();

    virtual uint32 GetBlockSize();
    virtual uint32 GetPoolCount();

    virtual uint32 GetCurrLevel();

    virtual uint16 GetLogID(uint16 u2Index);
    virtual const char*  GetLogInfoByServerName(uint16 u2LogID);
    virtual const char*  GetLogInfoByLogName(uint16 u2LogID);
    virtual int    GetLogInfoByLogDisplay(uint16 u2LogID);
    virtual uint16 GetLogInfoByLogLevel(uint16 u2LogID);

private:
    CLogFile**                     m_pLogFileList;
    char                           m_szLogRoot[MAX_BUFF_100];
    int                            m_nCount;

    uint32                         m_u4BlockSize;
    uint32                         m_u4PoolCount;
    uint32                         m_u4CurrLogLevel;
};

#endif
