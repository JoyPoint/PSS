#ifndef _BUFFPACKET_H
#define _BUFFPACKET_H

#include "ace/Thread_Mutex.h"

#include "ACEMemory.hpp"
#include "IBuffPacket.h"
#include <stdexcept>

const uint32 DEFINE_PACKET_SIZE      = 1024;
const uint32 DEFINE_PACKET_ADD       = 1024;
const uint8  USER_PACKET_MEMORY_POOL = 1; //定义使用ACE内存池分配

class CBuffPacket : public IBuffPacket
{
public:
    CBuffPacket();
    ~CBuffPacket(void);

    virtual uint32 GetPacketSize();    //得到数据包的格式化长度
    virtual uint32 GetPacketLen();     //得到数据包的实际长度
    virtual uint32 GetReadLen();       //得到包读取的长度
    virtual uint32 GetWriteLen();      //得到包写入的长度
    virtual uint32 GetHeadLen();       //得到数据包头的长度
    virtual uint32 GetPacketCount();   //得到缓存数据包的个数
    virtual const char*  GetData() const;  //得到当前数据指针

    bool Init(int32 nSize = DEFINE_PACKET_SIZE, int32 nMaxBuffSize = MAX_PACKET_SIZE);
    virtual bool Close();              //删除已经使用的内存
    virtual bool Clear();              //清除所有的标志位，并不删除内存。

    virtual bool WriteStream(const char* pData, uint32 u4Len);
    virtual bool ReadStream(char* pData, uint32& u4Len);
    virtual char* ReadPtr();                            //获得读指针
    virtual char* WritePtr();                           //获得写指针
    virtual void SetReadPtr(uint32 u4Pos);              //设置读指针的位置
    virtual void SetPacketCount(uint32 u4PacketCount);  //设置缓存数据包的个数
    virtual bool RollBack(uint32 u4Len);                //将取出的数据删除，将后面的数据加上
    virtual void ReadBuffPtr(uint32 u4Size);
    virtual void WriteBuffPtr(uint32 u4Size);

    bool AddBuff(uint32 u4Size);                //增加Packet的包大小


    void SetNetSort(bool blState);              //设置字节序开启开关，false为主机字序，true为网络字序

    void SetBuffID(uint32 u4BuffID);            //设置BuffID
    uint32 GetBuffID() const;                   //得到BuffID

    int32 GetHashID() const;                    //设置HashID
    void SetHashID(int32 nHashID);              //得到HashID

    const char* GetError() const;               //返回错误信息

private:
    void ReadPtr(uint32 u4Size);
    void WritePtr(uint32 u4Size);


public:
    //读取
    virtual CBuffPacket& operator >> (uint8& u1Data);
    virtual CBuffPacket& operator >> (uint16& u2Data);
    virtual CBuffPacket& operator >> (uint32& u4Data);
    virtual CBuffPacket& operator >> (uint64& u8Data);
    virtual CBuffPacket& operator >> (int8& n1Data);
    virtual CBuffPacket& operator >> (int16& n2Data);
    virtual CBuffPacket& operator >> (int32& n8Data);

    virtual CBuffPacket& operator >> (float32& f4Data);
    virtual CBuffPacket& operator >> (float64& f8Data);

    virtual CBuffPacket& operator >> (VCHARS_STR& str);
    virtual CBuffPacket& operator >> (VCHARM_STR& str);
    virtual CBuffPacket& operator >> (VCHARB_STR& str);
    virtual CBuffPacket& operator >> (string& str);

    //写入
    virtual CBuffPacket& operator << (uint8 u1Data);
    virtual CBuffPacket& operator << (uint16 u2Data);
    virtual CBuffPacket& operator << (uint32 u4Data);
    virtual CBuffPacket& operator << (uint64 u8Data);
    virtual CBuffPacket& operator << (int8 u1Data);
    virtual CBuffPacket& operator << (int16 u2Data);
    virtual CBuffPacket& operator << (int32 u4Data);

    virtual CBuffPacket& operator << (float32 f4Data);
    virtual CBuffPacket& operator << (float64 f8Data);

    virtual CBuffPacket& operator << (VCHARS_STR& str);
    virtual CBuffPacket& operator << (VCHARM_STR& str);
    virtual CBuffPacket& operator << (VCHARB_STR& str);
    virtual CBuffPacket& operator << (string& str);

private:
    char*                      m_szData          = NULL;                //数据指针
    uint32                     m_u4ReadPtr       = 0;                   //读包的位置
    uint32                     m_u4WritePtr      = 0;                   //写包的位置
    uint32                     m_u4PacketLen     = 0 ;                  //包总长度
    uint32                     m_u4PacketCount   = 0;                   //当前数据包的个数
    uint32                     m_u4MaxPacketSize = 0;                   //最大当前BuffPacket的容量
    uint32                     m_u4BuffID        = 0;                   //Buffer对象名字，用于核对内存池数据使用，寻找内存泄露。
    int32                      m_nHashID         = 0;                   //记录当前对象在Hash数组中的位置
    bool                       m_blNetSort       = false;               //字节序开启开关，false为不转换为主机字节序，true为转换为主机字节序
    char                       m_szError[MAX_BUFF_500] = {'\0'};        //错误信息

public:
    void* operator new(size_t stSize)
    {
        return App_ACEMemory::instance()->malloc(stSize);
    }

    void operator delete(void* p)
    {
        App_ACEMemory::instance()->free(p);
    }
};
#endif
