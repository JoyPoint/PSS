#ifndef _IPACCOUNT_H
#define _IPACCOUNT_H

//添加对客户端链接的统计信息
//add by freeeyes
//2016-11-25
//这里采用Hash数组的方式替代
//默认是当前最大连接的2倍

#include <string>
#include "ace/Date_Time.h"
#include "define.h"
#include "HashTable.h"
#include "ace/Recursive_Thread_Mutex.h"

//IP访问统计模块
class _IPAccount
{
public:
    int32          m_nCount    = 0;          //当前链接次数
    int32          m_nAllCount = 0;          //指定IP链接次数总和
    int32          m_nMinute   = 0;          //当前分钟数
    string         m_strIP;                  //当前链接地址
    ACE_Date_Time  m_dtLastTime;             //最后链接时间

    _IPAccount();

    void Add(ACE_Date_Time const& dtNowTime);

    //false为数据已过期
    bool Check(ACE_Date_Time const& dtNowTime) const;
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
    //定时清理Hash数组
    void Clear_Hash_Data(uint16 u2NowTime, const ACE_Date_Time& dtNowTime);

    uint32                           m_u4MaxConnectCount  = 0;                 //每秒允许的最大连接数，前提是m_nNeedCheck = 0;才会生效
    uint32                           m_u4CurrConnectCount = 0;                 //当前连接总数
    uint32                           m_u4LastConnectCount = 0;                 //之前一分钟的连接总数记录
    uint16                           m_u2CurrTime         = 0;                 //当前时间
    uint8                            m_u1Minute           = 0;                 //当前分钟数
    CHashTable<_IPAccount>           m_objIPList;                              //IP统计信息
    ACE_Recursive_Thread_Mutex       m_ThreadLock;                             //多线程锁
};

typedef ACE_Singleton<CIPAccount, ACE_Recursive_Thread_Mutex> App_IPAccount;

//单位时间连接数统计
class CConnectAccount
{
public:
    CConnectAccount();

    ~CConnectAccount();

    uint32 Get4ConnectMin() const;

    uint32 GetConnectMax() const;

    uint32 GetDisConnectMin() const;

    uint32 GetDisConnectMax() const;

    uint32 GetCurrConnect() const;

    uint32 GetCurrDisConnect() const;

    void Init(uint32 u4ConnectMin, uint32 u4ConnectMax, uint32 u4DisConnectMin, uint32 u4DisConnectMax);

    bool AddConnect();

    bool AddDisConnect();

    int32 CheckConnectCount() const;

    int32 CheckDisConnectCount() const;

private:
    uint32 m_u4CurrConnect    = 0;
    uint32 m_u4CurrDisConnect = 0;
    uint32 m_u4ConnectMin     = 0;
    uint32 m_u4ConnectMax     = 0;
    uint32 m_u4DisConnectMin  = 0;
    uint32 m_u4DisConnectMax  = 0;
    uint8  m_u1Minute         = 0;   //当前分钟数
};

typedef ACE_Singleton<CConnectAccount, ACE_Recursive_Thread_Mutex> App_ConnectAccount;
#endif
