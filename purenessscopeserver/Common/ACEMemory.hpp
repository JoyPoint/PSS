#ifndef _ACEMEMORY_H
#define _ACEMEMORY_H

#include "define.h"

//��ʱ����ACE_LOCAL_MEMORY_POOL, ʵ��ʹ��Ч�ʴ������⡣
//��new �� delete �滻֮

class CACEMemory
{
public:
    CACEMemory(void)
    {
        OUR_DEBUG((LM_ERROR, "[CACEMemory::CACEMemory].\n"));
    };

    ~CACEMemory(void)
    {
        OUR_DEBUG((LM_ERROR, "[CACEMemory::~CACEMemory]End.\n"));
    };

    void* malloc(size_t stSize)
    {
        return new char[stSize];
    };

    bool  free(void* p)
    {
        delete (char*)(p);
        return true;
    };
};

typedef ACE_Singleton<CACEMemory, ACE_Null_Mutex> App_ACEMemory;
#endif
