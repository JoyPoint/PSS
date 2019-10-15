#ifndef _BASESERVERMANAGER_H
#define _BASESERVERMANAGER_H

//ʵ��ServerManager�����й���������
//add by freeeyes

#include "define.h"
#include "ForbiddenIP.h"
#include "MessageService.h"
#include "LoadModule.h"
#include "LogManager.h"
#include "FileLogger.h"
#include "IObject.h"
#include "ModuleMessageManager.h"
#include "FrameCommand.h"
#include "BuffPacketManager.h"
#include "Frame_Logging_Strategy.h"
#include "IControlListen.h"
#include "XmlConfig.h"
#include "Echartlog.h"

//��ʼ��ACE�������
bool Server_Manager_Common_FrameLogging(Frame_Logging_Strategy*& pFrameLoggingStrategy);

//��ʼ����־ϵͳ
bool Server_Manager_Common_LogSystem();

//��ʼ�����������ϵͳ
bool Server_Manager_Common_Pool();

//��ʼ��������еĽӿڶ���
bool Server_Manager_Common_IObject(IConnectManager* pConnectManager,
                                   IClientManager* pClientManager,
                                   IUDPConnectManager* pUDPConnectManager,
                                   IFrameCommand* pFrameCommand,
                                   IServerManager* pIServerManager,
                                   ITMService* pTMService,
                                   ITTyClientManager* pTTyClientManager,
                                   IMessageQueueManager* pLogicThreadManager,
                                   IControlListen* pControlListen);

//��ʼ��ģ�����ļ���
bool Server_Manager_Common_Module();

//ת��Addr IP�ӿ�
bool Server_Manager_Common_Addr(uint8 u4IpType, const char* pIP, uint32 u4Port, ACE_INET_Addr& listenAddr);

#endif
