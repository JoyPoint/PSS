#ifndef _ACEMEMORY_H
#define _ACEMEMORY_H

#include "define.h"

//��ʱ����ACE_LOCAL_MEMORY_POOL, ʵ��ʹ��Ч�ʴ������⡣
//��new �� delete �滻֮

class CACEMemory
{
public:
    CACEMemory(void);
    ~CACEMemory(void);

    void* malloc(size_t stSize);
    bool  free(void* p);
};

typedef ACE_Singleton<CACEMemory, ACE_Null_Mutex> App_ACEMemory;
#endif
