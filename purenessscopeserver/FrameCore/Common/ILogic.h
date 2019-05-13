#pragma once
#ifndef ILOGIC_H
#define ILOGIC_H

#include "define.h"
#include <string>

//add �빤���߳��޹ص�ҵ����Ϣ���е��߼�ҵ����,����̳к�ʵ�� liuruiqi

class ILogic
{
public:
	//��ʼ������ ����:�߼�ID, ����, ���ʱ��(��)
	ILogic(uint32 nLogicID, const char* descriptor, uint32 nTimeout);
	virtual ~ILogic();
public:
	//��ȡ�߳�ID
	uint32 GetLogicThreadID();
	//��ʱ
	void SetTimeOut(uint32 nTimeout);
	uint32 GetTimeOut();
	//����
	std::string GetDescriptor();
public:
	//��ʼ������
	virtual bool Init() = 0;
	//ִ���߼�
	virtual uint32 Run() = 0;
	//������
	virtual uint32 Error() = 0;
	//�˳��ƺ�
	virtual void Exit() = 0;
private:
	uint32 uLogicThreadID;
	uint32 uTimeout;
	std::string des;
};


#endif // ILOGIC_H