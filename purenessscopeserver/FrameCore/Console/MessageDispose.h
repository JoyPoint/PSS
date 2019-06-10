#ifndef _MESSAGEDISPOSE_H
#define _MESSAGEDISPOSE_H

//���ﴦ�����еĽ������������
//��Щ������Ծ�̬�����ķ�ʽ��¶����
//���������ڴ�����Ķ�����Ϣ��ӳ��
//��ô����ԭ���ǣ����������if else ��������
//�Ż�����ṹ������sonar��������
//add by freeeyes

#include "MessageDefine.h"
#include "IBuffPacket.h"
#include "LoadModule.h"
#include "MessageManager.h"
#include "MessageService.h"
#include "MakePacket.h"
#include "ace/Message_Block.h"
#include "IPAccount.h"
#include "IObject.h"
#include "FileTestManager.h"
#include "ConsolePromiss.h"
#include "XmlConfig.h"

#if PSS_PLATFORM == PLATFORM_WIN
#include "ProConnectHandler.h"
#include "ClientProConnectManager.h"
#include "ProUDPManager.h"
#include "WindowsCPU.h"
#include "ProControlListen.h"
#else
#include "ConnectHandler.h"
#include "ClientReConnectManager.h"
#include "ReUDPManager.h"
#include "LinuxCPU.h"
#include "ControlListen.h"
#endif

//���ö�Ӧ�����������ƣ����ڽ��ն˰�����
const uint16 CONSOLE_COMMAND_UNKNOW             = 0x1000;
const uint16 CONSOLE_COMMAND_LOADMOUDLE         = 0x1001;
const uint16 CONSOLE_COMMAND_UNLOADMOUDLE       = 0x1002;
const uint16 CONSOLE_COMMAND_RELOADMOUDLE       = 0x1003;
const uint16 CONSOLE_COMMAND_SHOWMOUDLE         = 0x1004;
const uint16 CONSOLE_COMMAND_CLIENTCOUNT        = 0x1005;
const uint16 CONSOLE_COMMAND_COMMANDINFO        = 0x1006;
const uint16 CONSOLE_COMMAND_COMMANDTIMEOUT     = 0x1007;
const uint16 CONSOLE_COMMAND_COMMANDTIMEOUTCLR  = 0x1008;
const uint16 CONSOLE_COMMAND_COMMANDDATALOG     = 0x1009;
const uint16 CONSOLE_COMMAND_THREADINFO         = 0x100A;
const uint16 CONSOLE_COMMAND_CLIENTINFO         = 0x100B;
const uint16 CONSOLE_COMMAND_UDPCONNECTINFO     = 0x100C;
const uint16 CONSOLE_COMMAND_COLSECLIENT        = 0x100D;
const uint16 CONSOLE_COMMAND_FORBIDDENIP        = 0x100E;
const uint16 CONSOLE_COMMAND_FORBIDDENIPSHOW    = 0x100F;
const uint16 CONSOLE_COMMAND_LIFTED             = 0x1010;
const uint16 CONSOLE_COMMAND_SERVERCONNECT_TCP  = 0x1011;
const uint16 CONSOLE_COMMAND_SERVERCONNECT_UDP  = 0x1012;
const uint16 CONSOLE_COMMAND_PROCESSINFO        = 0x1013;
const uint16 CONSOLE_COMMAND_CLIENTHISTORY      = 0x1014;
const uint16 CONSOLE_COMMAND_ALLCOMMANDINFO     = 0x1015;
const uint16 CONSOLE_COMMAND_SERVERINFO         = 0x1016;
const uint16 CONSOLE_COMMAND_SERVERRECONNECT    = 0x1017;
const uint16 CONSOLE_COMMAND_SETDEBUG           = 0x1018;
const uint16 CONSOLE_COMMAND_SHOWDEBUG          = 0x1019;
const uint16 CONSOLE_COMMAND_SETTRACKIP         = 0x101A;
const uint16 CONSOLE_COMMAND_SETTRACECOMMAND    = 0x101B;
const uint16 CONSOLE_COMMAND_GETTRACKCOMMAND    = 0x101C;
const uint16 CONSOLE_COMMAND_GETCONNECTIPINFO   = 0x101D;
const uint16 CONSOLE_COMMAND_GETLOGINFO         = 0x101E;
const uint16 CONSOLE_COMMAND_SETLOGLEVEL        = 0x101F;
const uint16 CONSOLE_COMMAND_GETWTAI            = 0x1020;
const uint16 CONSOLE_COMMAND_GETWTTIMEOUT       = 0x1021;
const uint16 CONSOLE_COMMAND_SETWTAI            = 0x1022;
const uint16 CONSOLE_COMMAND_GETNICKNAMEINFO    = 0x1023;
const uint16 CONSOLE_COMMAND_SETCONNECTLOG      = 0x1024;
const uint16 CONSOLE_COMMAND_SETMAXCONNECTCOUNT = 0x1025;
const uint16 CONSOLE_COMMAND_ADD_LISTEN         = 0x1026;
const uint16 CONSOLE_COMMAND_DEL_LISTEN         = 0x1027;
const uint16 CONSOLE_COMMAND_SHOW_LISTEN        = 0x1028;
const uint16 CONSOLE_COMMAND_MONITOR_INFO       = 0x1029;
const uint16 CONSOLE_COMMAND_CLOSE_SERVER       = 0x1030;
const uint16 CONSOLE_COMMAND_FILE_TEST_START    = 0x1031;
const uint16 CONSOLE_COMMAND_FILE_TEST_STOP     = 0x1032;
const uint16 CONSOLE_COMMAND_PORT_FLOW          = 0x1033;
const uint16 CONSOLE_COMMAND_PACKET_STATE       = 0x1034;
const uint16 CONSOLE_COMMAND_POOL_SET           = 0x1035;

//Ŀǰ֧�ֵ�����
static const char* CONSOLEMESSAHE_LOADMODULE = "LoadModule";          //����ģ��
static const char* CONSOLEMESSAHE_UNLOADMODULE = "UnLoadModule";        //ж��ģ��
static const char* CONSOLEMESSAHE_RELOADMODULE = "ReLoadModule";        //���¼���ģ��
static const char* CONSOLEMESSAHE_SHOWMOUDLE = "ShowModule";          //��ʾ���������Ѿ����ص�ģ��
static const char* CONSOLEMESSAHE_CLIENTCOUNT = "ClientCount";         //��ǰ�ͻ���������
static const char* CONSOLEMESSAHE_COMMANDINFO = "CommandInfo";         //��ǰĳһ�������״̬��Ϣ
static const char* CONSOLEMESSAHE_COMMANDTIMEOUT = "CommandTimeout";      //���г�ʱ�������б�
static const char* CONSOLEMESSAHE_COMMANDTIMEOUTCLR = "CommandTimeoutclr";   //�����ʱ�������б�
static const char* CONSOLEMESSAGE_COMMANDDATALOG = "CommandDataLog";     //�洢CommandDataLog
static const char* CONSOLEMESSAHE_THREADINFO = "WorkThreadState";     //��ǰ�����̺߳͹����߳�״̬
static const char* CONSOLEMESSAHE_CLIENTINFO = "ConnectClient";       //��ǰ�ͻ������ӵ���Ϣ
static const char* CONSOLEMESSAHE_UDPCONNECTINFO = "UDPConnectClient";    //��ǰUDP�ͻ��˵�������Ϣ
static const char* CONSOLEMESSAHE_COLSECLIENT = "CloseClient";         //�رտͻ���
static const char* CONSOLEMESSAHE_FORBIDDENIP = "ForbiddenIP";         //��ֹIP����
static const char* CONSOLEMESSAHE_FORBIDDENIPSHOW = "ShowForbiddenIP";     //�鿴��ֹ����IP�б�
static const char* CONSOLEMESSAHE_LIFTED = "LiftedIP";            //���ĳIP
static const char* CONSOLEMESSAHE_SERVERCONNECT_TCP = "ServerConnectTCP";    //��������ͨѶ(TCP)
static const char* CONSOLEMESSAHE_SERVERCONNECT_UDP = "ServerConnectUDP";    //��������ͨѶ(UDP)
static const char* CONSOLEMESSAGE_PROCESSINFO = "ShowCurrProcessInfo"; //�鿴��ǰ������������״̬ShowServerInfo
static const char* CONSOLEMESSAGE_CLIENTHISTORY = "ShowConnectHistory";  //�鿴��������ʷ����״̬
static const char* CONSOLEMESSAGE_ALLCOMMANDINFO = "ShowAllCommand";      //�鿴����������ע��ģ��������Ϣ
static const char* CONSOLEMESSAGE_SERVERINFO = "ShowServerInfo";      //�鿴������������Ϣ
static const char* CONSOLEMESSAGE_SERVERRECONNECT = "ReConnectServer";     //Զ�˿�������ĳһ��Զ�˷�����
static const char* CONSOLEMESSAGE_SETDEBUG = "SetDebug";            //���õ�ǰDEBUG״̬
static const char* CONSOLEMESSAGE_SHOWDEBUG = "ShowDebug";           //�鿴��ǰDEBUG״̬
static const char* CONSOLEMESSAGE_SETTRACKIP = "SetTrackIP";          //����ҪȾɫ��IP
static const char* CONSOLEMESSAGE_SETTRACECOMMAND = "SetTrackCommand";    //����ҪȾɫ��CommandID
static const char* CONSOLEMESSAGE_GETTRACKIPINFO = "GetTrackCommandInfo"; //�õ�Ⱦɫ��CommandID��Ϣ
static const char* CONSOLEMESSAGE_GETCONNECTIPINFO = "GetConnectIPInfo";    //ͨ��ConnectID�����ص�IP��Ϣ
static const char* CONSOLEMESSAGE_GETLOGINFO = "GetLogInfo";          //�õ���־�ȼ�
static const char* CONSOLEMESSAGE_SETLOGLEVEL = "SetLogLevel";         //������־�ȼ�
static const char* CONSOLEMESSAGE_GETWTAI = "GetWorkThreadAI";     //�õ�Thread��AI������Ϣ
static const char* CONSOLEMESSAGE_GETWTTIMEOUT = "GetWorkThreadTO";     //�õ�Thread�����г�ʱ���ݰ���Ϣ
static const char* CONSOLEMESSAGE_SETWTAI = "SetWorkThreadAI";     //����ThreadAI��������Ϣ
static const char* CONSOLEMESSAGE_GETNICKNAMEINFO = "GetNickNameInfo";     //�õ�������Ϣ
static const char* CONSOLEMESSAGE_SETCONNECTLOG = "SetConnectLog";       //����������־����״̬
static const char* CONSOLEMESSAGE_SETMAXCONNECTCOUNT = "SetMaxConnectCount";  //�������������
static const char* CONSOLEMESSAGE_ADD_LISTEN = "AddListen";           //���һ���µļ����˿�
static const char* CONSOLEMESSAGE_DEL_LISTEN = "DelListen";           //ɾ��һ���µļ����˿�
static const char* CONSOLEMESSATE_SHOW_LISTEN = "ShowListen";          //�鿴���ڴ򿪵ļ����˿�
static const char* CONSOLEMESSATE_MONITOR_INFO = "Monitor";             //���������в����ӿ�
static const char* CONSOLEMESSATE_SERVER_CLOSE = "ServerClose";         //�رյ�ǰ������
static const char* CONSOLEMESSATE_FILE_TEST_START = "TestFileStart";       //�����������ļ���������
static const char* CONSOLEMESSATE_FILE_TEST_STOP = "TestFileStop";        //ֹͣ�������ļ���������
static const char* CONSOLEMESSATE_PORT_FLOW = "PortFlow";            //�õ���ǰʱ��Ķ˿���������
static const char* CONSOLEMESSATE_PACKET_STATE = "BuffPacketState";     //��õ�ǰ����ʹ�õ�BuffPacket���
static const char* CONSOLEMESSATE_POOL_SET = "PoolSet";             //�����ڴ�ش�����Ϣ���λ�Ƿ���

//ͳһ��������ƴװ�ӿ�
void Combo_Common_Return_Data(uint8 u1OutputType, uint8 u1State, const char* pMessage, IBuffPacket* pBuffPacket);
void Combo_Common_Head_Data(uint8 u1OutputType, uint32 u4Count, const char* pMessage, IBuffPacket* pBuffPacket);
void Combo_Common_vecClientConnectInfo(uint8 u1OutputType, vecClientConnectInfo& VecClientConnectInfo, IBuffPacket* pBuffPacket);
void Combo_Common_VecForbiddenIP(uint8 u1OutputType, VecForbiddenIP* pIPList, IBuffPacket* pBuffPacket);

//Э����Ϣ����������
void DoMessage_LoadModule(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_UnLoadModule(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_ReLoadModule(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_ClientMessageCount(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_ShowModule(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_CommandInfo(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_WorkThreadState(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_ClientInfo(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_CloseClient(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_ForbiddenIP(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_ShowForbiddenList(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_LifedIP(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_UDPClientInfo(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_ServerConnectTCP(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_ServerConnectUDP(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_ShowProcessInfo(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_ShowClientHisTory(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_ShowAllCommandInfo(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_ShowServerInfo(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_ReConnectServer(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_CommandTimeout(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_CommandTimeoutclr(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_CommandDataLog(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_SetDebug(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_ShowDebug(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_SetTrackIP(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_SetTraceCommand(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_GetTrackCommand(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_GetConnectIPInfo(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_GetLogLevelInfo(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_SetLogLevelInfo(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_GetThreadAI(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_GetWorkThreadTO(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_SetWorkThreadAI(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_GetNickNameInfo(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_SetConnectLog(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_SetMaxConnectCount(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_AddListen(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_DelListen(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_ShowListen(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_MonitorInfo(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_ServerClose(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_TestFileStart(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_TestFileStop(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void DoMessage_PortList(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void Do_Message_BuffPacket(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);
void Do_Message_PoolSet(_CommandInfo& CommandInfo, IBuffPacket* pBuffPacket, uint16& u2ReturnCommandID);

#endif
