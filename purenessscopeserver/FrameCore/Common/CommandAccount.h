#ifndef _COMMANDACCOUNT_K
#define _COMMANDACCOUNT_K

// ͳ�����н�����ܵ�����ִ�����
// ��ͳ�����еĽ�������
// add by freeeyes
// 2012-03-19

#include "ace/Date_Time.h"
#include "define.h"
#include "LogManager.h"
#include "HashTable.h"

//ͳ����Ϣ��������Ҫͳ�Ƶ�������Ϣ����
class _CommandData
{
public:
    uint64 m_u8CommandCost;                //�����ִ�кķ���ʱ��
    uint32 m_u4CommandCount;               //������ܵ��ô���
    uint32 m_u4PacketSize;                 //���������������
    uint16 m_u2CommandID;                  //�����ID
    uint8  m_u1PacketType;                 //���ݰ���Դ����
    uint8  m_u1CommandType;                //��������ͣ�0���յ������1�Ƿ���������
    ACE_Time_Value m_tvCommandTime;        //����������ʱ��

    _CommandData()
    {
        m_u2CommandID    = 0;
        m_u4CommandCount = 0;
        m_u8CommandCost  = 0;
        m_u4PacketSize   = 0;
        m_u1PacketType   = CONNECT_IO_TCP;
        m_u1CommandType  = COMMAND_TYPE_IN;
    }

    _CommandData& operator = (const _CommandData& ar)
    {
        if (this != &ar)
        {
            this->m_u2CommandID = ar.m_u2CommandID;
            this->m_u4CommandCount = ar.m_u4CommandCount;
            this->m_u8CommandCost = ar.m_u8CommandCost;
            this->m_u1CommandType = ar.m_u1CommandType;
            this->m_u4PacketSize = ar.m_u4PacketSize;
            this->m_u1PacketType = ar.m_u1PacketType;
            this->m_tvCommandTime = ar.m_tvCommandTime;
        }

        return *this;
    }

    _CommandData& operator += (const _CommandData& ar)
    {
        if(this->m_u2CommandID != ar.m_u2CommandID)
        {
            this->m_u2CommandID = ar.m_u2CommandID;
        }

        this->m_u4CommandCount += ar.m_u4CommandCount;
        this->m_u8CommandCost  += ar.m_u8CommandCost;
        this->m_u1CommandType  += ar.m_u1CommandType;
        this->m_u4PacketSize   += ar.m_u4PacketSize;
        this->m_u1PacketType   += ar.m_u1PacketType;
        this->m_tvCommandTime  += ar.m_tvCommandTime;
        return *this;
    }

    ~_CommandData() = default;
};

class _CommandAlertData
{
public:
    uint32 m_u4CommandCount;
    uint32 m_u4MailID;
    uint32 m_u4CurrCount;
    uint16 m_u2CommandID;
    uint8  m_u1Minute;

    _CommandAlertData()
    {
        m_u2CommandID    = 0;
        m_u4CommandCount = 0;
        m_u4MailID       = 0;
        m_u4CurrCount    = 0;
        m_u1Minute       = 0;
    }
};
typedef vector<_CommandAlertData> vecCommandAlertData;   //��¼���еĸ澯��ط�ֵ

//��Ӧ�˿����ݽ�����Ϣ
class _Port_Data_Account
{
public:
    uint8                       m_u1Type;                        //��ǰ��������
    uint32                      m_u4Port;                        //��ǰ���ݶ˿�
    uint8                       m_u1Minute;                      //��õ�ǰ������
    uint32                      m_u4FlowIn;                      //��ǰ��������ͳ��(��λ������)
    uint32                      m_u4FlowOut;                     //��ǰ��������ͳ��(��λ������)

    _Port_Data_Account()
    {
        m_u1Type       = CONNECT_IO_TCP;
        m_u4Port       = 0;
        m_u4FlowIn     = 0;
        m_u4FlowOut    = 0;
        m_u1Minute     = 0;
    }

    ~_Port_Data_Account() = default;

    _Port_Data_Account& operator = (const _Port_Data_Account& ar)
    {
        if (this != &ar)
        {
            this->m_u1Type = ar.m_u1Type;
            this->m_u4Port = ar.m_u4Port;
            this->m_u4FlowIn = ar.m_u4FlowIn;
            this->m_u4FlowOut = ar.m_u4FlowOut;
            this->m_u1Minute = ar.m_u1Minute;
        }

        return *this;
    }

    _Port_Data_Account& operator += (const _Port_Data_Account& ar)
    {
        if ((this->m_u4Port == ar.m_u4Port) && (this->m_u1Minute == ar.m_u1Minute))
        {
            this->m_u4FlowIn += ar.m_u4FlowIn;
            this->m_u4FlowOut += ar.m_u4FlowOut;
        }

        return *this;
    }

    //��ʼ������
    void Init(uint8 u1Type, uint32 u4Port)
    {
        m_u1Type = u1Type;
        m_u4Port = u4Port;
    }

    uint32 GetFlowIn()
    {
        //�õ��˿̵�����������
        ACE_Date_Time dtNowTime(ACE_OS::gettimeofday());
        uint8 u1Minute = (uint8)dtNowTime.minute();

        if (u1Minute != m_u1Minute)
        {
            m_u4FlowIn  = 0;
            m_u4FlowOut = 0;
            m_u1Minute  = u1Minute;
            return m_u4FlowIn;
        }
        else
        {
            return m_u4FlowIn;
        }
    }

    uint32 GetFlowOut()
    {
        //�õ��˿̵�����������
        ACE_Date_Time dtNowTime(ACE_OS::gettimeofday());
        uint8 u1Minute = (uint8)dtNowTime.minute();

        if (u1Minute != m_u1Minute)
        {
            m_u4FlowIn  = 0;
            m_u4FlowOut = 0;
            m_u1Minute  = u1Minute;
            return m_u4FlowOut;
        }
        else
        {
            return m_u4FlowOut;
        }
    }

    void SetFlow(uint8 u1CommandType, uint32 u4Size, ACE_Time_Value const& tvNow)
    {
        //��¼�˿�����
        ACE_Date_Time dtNowTime(tvNow);
        uint8 u1Minute = (uint8)dtNowTime.minute();

        if (u1Minute != m_u1Minute)
        {
            if (COMMAND_TYPE_IN == u1CommandType)
            {
                m_u4FlowIn  = u4Size;
                m_u4FlowOut = 0;
            }
            else
            {
                m_u4FlowIn  = 0;
                m_u4FlowOut = u4Size;
            }

            m_u1Minute = u1Minute;
        }
        else
        {
            if (COMMAND_TYPE_IN == u1CommandType)
            {
                m_u4FlowIn += u4Size;
            }
            else
            {
                m_u4FlowOut += u4Size;
            }
        }
    }
};
typedef vector<_Port_Data_Account> vecPortDataAccount;

//�ϲ������
void Combo_Port_List(vecPortDataAccount& vec_Port_Data_Account, vecPortDataAccount& vec_Port_Data_All_Account);

//ͳ�����н�����ܵ�����ִ�������Ŀǰ�����������������������ͳ�ƣ���Ϊ�ⲿ��Э���޷�ͳһ��
class CCommandAccount
{
public:
    CCommandAccount();
    ~CCommandAccount();

    CCommandAccount& operator = (const CCommandAccount& ar)
    {
        if (this != &ar)
        {
            ACE_UNUSED_ARG(ar);
        }

        return *this;
    }

    void InitName(const char* pName, uint32 u4CommandCount);
    void Init(uint8 u1CommandAccount, uint8 u1Flow, uint16 u2RecvTimeout);
    void AddCommandAlert(uint16 u2CommandID, uint32 u4Count, uint32 u4MailID);

    bool SaveCommandData(uint16 u2CommandID, uint32 u4Port, uint8 u1PacketType = CONNECT_IO_TCP,
                         uint32 u4PacketSize = 0, uint8 u1CommandType = COMMAND_TYPE_IN,
                         ACE_Time_Value const& tvTime = ACE_OS::gettimeofday());   //��¼����ִ����Ϣ
    bool SaveCommandDataLog();                         //�洢����ִ����Ϣ����־
    _CommandData* GetCommandData(uint16 u2CommandID);  //���ָ��������������

    uint32 GetFlowIn();                                //�õ���λʱ���������
    uint32 GetFlowOut();                               //�õ���ίʱ����������
    uint8  GetFLow();                                  //�õ���ǰ��������״̬

    void GetCommandAlertData(vecCommandAlertData& CommandAlertDataList); //�õ����еĸ澯������Ϣ

    void GetFlowPortList(vector<_Port_Data_Account>& vec_Port_Data_Account);    //���ݲ�ͬ�ļ����˿ڣ���õ�ǰ�Ķ˿ڶ�Ӧ�ĳ����������Ϣ

    void Close();

private:
    bool Save_Flow(uint16 u2CommandID, uint32 u4Port, uint8 u1PacketType = CONNECT_IO_TCP,
                   uint32 u4PacketSize = 0, uint8 u1CommandType = COMMAND_TYPE_IN,
                   ACE_Time_Value const& tvTime = ACE_OS::gettimeofday());                    //����ͳ��

    bool Save_Command(uint16 u2CommandID, uint32 u4Port, uint8 u1PacketType = CONNECT_IO_TCP,
                      uint32 u4PacketSize = 0, uint8 u1CommandType = COMMAND_TYPE_IN,
                      ACE_Time_Value const& tvTime = ACE_OS::gettimeofday());                 //����ͳ��

    bool Save_Alert(uint16 u2CommandID, uint32 u4Port, uint8 u1PacketType = CONNECT_IO_TCP,
                    uint32 u4PacketSize = 0, uint8 u1CommandType = COMMAND_TYPE_IN,
                    ACE_Time_Value const& tvTime = ACE_OS::gettimeofday());                 //����澯ͳ��

public:
    uint64                                    m_u8PacketTimeout;               //������ʱʱ��
    uint8                                     m_u1CommandAccount;              //�Ƿ�������ͳ�ƣ�1�Ǵ򿪣�0�ǹر�
    uint8                                     m_u1Flow;                        //�Ƿ������ͳ�ƣ�1�Ǵ򿪣�0�ǹر�
    uint8                                     m_u1Minute;                      //��ǰ������
    char                                      m_szName[MAX_BUFF_50];           //��ǰͳ�Ƶ�����
    CHashTable<_CommandData>                  m_objCommandDataList;            //����Hashӳ���б�
    vecCommandAlertData                       m_vecCommandAlertData;           //�澯��ֵ����
    ACE_Hash_Map<uint32, _Port_Data_Account*> m_objectPortAccount;             //���ݶ˿�ͳ��ÿ�����ݵĽ�����
};

#endif