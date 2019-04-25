#ifndef _MESSAGE_H
#define _MESSAGE_H

#include "ace/Singleton.h"
#include "ace/Thread_Mutex.h"
#include "MessageBlockManager.h"
#include "BuffPacket.h"
#include "IMessage.h"
#include "ObjectPoolManager.h"

class CMessage : public IMessage
{
public:
    CMessage(void);
    ~CMessage(void);

    void Close();
    void Clear();

    void SetHashID(int nHasnID);
    int  GetHashID();

    void SetMessageBase(_MessageBase* pMessageBase);

    ACE_Message_Block* GetMessageHead();
    ACE_Message_Block* GetMessageBody();

    _MessageBase* GetMessageBase();

    bool GetPacketHead(_PacketInfo& PacketInfo);
    bool GetPacketBody(_PacketInfo& PacketInfo);
    bool SetPacketHead(ACE_Message_Block* pmbHead);
    bool SetPacketBody(ACE_Message_Block* pmbBody);

    const char* GetError();

    ACE_Message_Block*  GetQueueMessage();

private:
    int           m_nHashID;
    char          m_szError[MAX_BUFF_500];
    _MessageBase* m_pMessageBase;

    ACE_Message_Block* m_pmbHead;             //��ͷ����
    ACE_Message_Block* m_pmbBody;             //���岿��

    ACE_Message_Block*  m_pmbQueuePtr;        //��Ϣ����ָ���
};


//Message�����
class CMessagePool : public CObjectPoolManager<CMessage, ACE_Recursive_Thread_Mutex>
{
public:
    CMessagePool();
    ~CMessagePool();

    CMessagePool(const CMessagePool& ar);

    CMessagePool& operator = (const CMessagePool& ar)
    {
        if (this != &ar)
        {
            ACE_UNUSED_ARG(ar);
        }

        return *this;
    }

    static void Init_Callback(int nIndex, CMessage* pMessage);
    static void Close_Callback(int nIndex, CMessage* pMessage);

    CMessage* Create();
    bool Delete(CMessage* pMakePacket);

    int GetUsedCount();
    int GetFreeCount();

    void GetCreateInfoList(vector<_Object_Create_Info>& objCreateList);
};

#endif