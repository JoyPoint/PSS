#ifndef _CONTROLLISTEN_H
#define _CONTROLLISTEN_H

#include "ConnectAccept.h"
#include "AceReactorManager.h"
#include "IControlListen.h"

class CControlListen : public IControlListen
{
public:
    CControlListen();
    virtual ~CControlListen();

    bool   AddListen(const char* pListenIP, uint16 u2Port, uint8 u1IPType, int nPacketParseID);  //��һ���µļ����˿�
    bool   DelListen(const char* pListenIP, uint16 u2Port);                                      //�ر�һ����֪������
    uint32 GetListenCount();                                                                     //�õ��Ѿ��򿪵ļ�������
    bool   ShowListen(uint32 u4Index, _ControlInfo& objControlInfo);                             //���������鿴�Ѵ򿪵ļ����˿�
    uint32 GetServerID();                                                                        //�õ�������ID
private:
    vecControlInfo m_vecListenList;
};

typedef ACE_Singleton<CControlListen, ACE_Null_Mutex> App_ControlListen;

#endif
