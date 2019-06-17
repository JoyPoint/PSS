#ifndef _FILETESTMANAGER_H_
#define _FILETESTMANAGER_H_

#ifndef WIN32
#include "ConnectHandler.h"
#else
//�����windows
#include "ProConnectHandler.h"
#endif

#include "HashTable.h"
#include "XmlOpeation.h"

#include "ace/FILE_Addr.h"
#include "ace/FILE_Connector.h"
#include "ace/FILE_IO.h"
#include "FileTest.h"

class CFileTestManager : public ACE_Task<ACE_MT_SYNCH>, public IFileTestManager
{
public:
    CFileTestManager(void);

    //�ļ����Է���
    FileTestResultInfoSt FileTestStart(const char* szXmlFileTestName);      //��ʼ�ļ�����
    int FileTestEnd();                                                      //�����ļ�����
    void HandlerServerResponse(uint32 u4ConnectID);                         //��ǰ���ӷ������ݰ��Ļص�����
    void Close();                                                           //����

    virtual int handle_timeout(const ACE_Time_Value& tv, const void* arg);

private:
    bool LoadXmlCfg(const char* szXmlFileTestName, FileTestResultInfoSt& objFileTestResult);        //��ȡ���������ļ�
    int  ReadTestFile(const char* pFileName, int nType, FileTestDataInfoSt& objFileTestDataInfo);   //����Ϣ���ļ��������ݽṹ
    int  InitResponseRecordList();                                                                  //��ʼ��ResponseRecord
    bool AddResponseRecordList(uint32 u4ConnectID, const ACE_Time_Value& tv);                       //���ResponseRecordList

    ACE_Recursive_Thread_Mutex  m_ThreadWriteLock;
    //�ļ����Ա���
    bool m_bFileTesting;          //�Ƿ����ڽ����ļ�����
    int32 m_n4TimerID;            //��ʱ��ID

    CXmlOpeation m_MainConfig;
    string m_strProFilePath;
    uint32 m_u4TimeInterval;      //��ʱ���¼����
    uint32 m_u4ConnectCount;      //ģ��������
    uint32 m_u4ResponseCount;     //������Ӧ������
    uint32 m_u4ExpectTime;        //��������ʱ�ܵĺ�ʱ(��λ����)
    uint32 m_u4ParseID;           //������ID
    uint32 m_u4ContentType;       //Э����������,1�Ƕ�����Э��,0���ı�Э��

    typedef vector<FileTestDataInfoSt> vecFileTestDataInfoSt;
    vecFileTestDataInfoSt m_vecFileTestDataInfoSt;

    //�ڲ�ʹ�õ�ӳ����
    class ResponseRecordSt
    {
    public:
        uint64 m_u8StartTime;
        uint8  m_u1ResponseCount;
        uint32 m_u4ConnectID;

        ResponseRecordSt()
        {
            Init();
        }

        void Init()
        {
            m_u8StartTime      = 0;
            m_u1ResponseCount  = 0;
            m_u4ConnectID      = 0;
        }

        ResponseRecordSt& operator= (const ResponseRecordSt& ar)
        {
            if (this != &ar)
            {
                this->m_u8StartTime = ar.m_u8StartTime;
                this->m_u1ResponseCount = ar.m_u1ResponseCount;
            }

            return *this;
        }
    } ;

    //������ղ���
    CHashTable<ResponseRecordSt> m_objResponseRecordList;

};

typedef ACE_Singleton<CFileTestManager, ACE_Null_Mutex> App_FileTestManager;
#endif //_FILETESTMANAGER_H_