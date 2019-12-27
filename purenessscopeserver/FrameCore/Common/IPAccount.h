#ifndef _IPACCOUNT_H
#define _IPACCOUNT_H

//��ӶԿͻ������ӵ�ͳ����Ϣ
//add by freeeyes
//2016-11-25
//�������Hash����ķ�ʽ���
//Ĭ���ǵ�ǰ������ӵ�2��

#include <string>
#include "ace/Date_Time.h"
#include "define.h"
#include "HashTable.h"
#include "ace/Recursive_Thread_Mutex.h"

//IP����ͳ��ģ��
class _IPAccount
{
public:
    int32          m_nCount    = 0;          //��ǰ���Ӵ���
    int32          m_nAllCount = 0;          //ָ��IP���Ӵ����ܺ�
    int32          m_nMinute   = 0;          //��ǰ������
    string         m_strIP;                  //��ǰ���ӵ�ַ
    ACE_Date_Time  m_dtLastTime;             //�������ʱ��

    _IPAccount();

    void Add(ACE_Date_Time const& dtNowTime);

    //falseΪ�����ѹ���
    bool Check(ACE_Date_Time const& dtNowTime);
};

typedef vector<_IPAccount> vecIPAccount;

class CIPAccount
{
public:
    CIPAccount();

    void Close();

    void Init(uint32 u4IPCount);

    bool AddIP(string strIP);

    int32 GetCount();

    uint32 GetLastConnectCount();

    void GetInfo(vecIPAccount& VecIPAccount);

private:
    //��ʱ����Hash����
    void Clear_Hash_Data(uint16 u2NowTime, const ACE_Date_Time& dtNowTime);

    uint32                           m_u4MaxConnectCount  = 0;                 //ÿ������������������ǰ����m_nNeedCheck = 0;�Ż���Ч
    uint32                           m_u4CurrConnectCount = 0;                 //��ǰ��������
    uint32                           m_u4LastConnectCount = 0;                 //֮ǰһ���ӵ�����������¼
    uint16                           m_u2CurrTime         = 0;                 //��ǰʱ��
    uint8                            m_u1Minute           = 0;                 //��ǰ������
    CHashTable<_IPAccount>           m_objIPList;                              //IPͳ����Ϣ
    ACE_Recursive_Thread_Mutex       m_ThreadLock;                             //���߳���
};

typedef ACE_Singleton<CIPAccount, ACE_Recursive_Thread_Mutex> App_IPAccount;

//��λʱ��������ͳ��
class CConnectAccount
{
public:
    CConnectAccount();

    ~CConnectAccount();

    uint32 Get4ConnectMin();

    uint32 GetConnectMax();

    uint32 GetDisConnectMin();

    uint32 GetDisConnectMax();

    uint32 GetCurrConnect();

    uint32 GetCurrDisConnect();

    void Init(uint32 u4ConnectMin, uint32 u4ConnectMax, uint32 u4DisConnectMin, uint32 u4DisConnectMax);

    bool AddConnect();

    bool AddDisConnect();

    int32 CheckConnectCount();

    int32 CheckDisConnectCount();

private:
    uint32 m_u4CurrConnect    = 0;
    uint32 m_u4CurrDisConnect = 0;
    uint32 m_u4ConnectMin     = 0;
    uint32 m_u4ConnectMax     = 0;
    uint32 m_u4DisConnectMin  = 0;
    uint32 m_u4DisConnectMax  = 0;
    uint8  m_u1Minute         = 0;   //��ǰ������
};

typedef ACE_Singleton<CConnectAccount, ACE_Recursive_Thread_Mutex> App_ConnectAccount;
#endif
