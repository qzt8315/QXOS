// 内存相关
#ifndef __MEM_H__
#define __MEM_H__
#include "type.h"

typedef struct s_ards
{
    u32 BaseAddrLow;
    u32 BaseAddrHigh;
    u32 LengthLow;
    u32 LengthHigh;
    u32 type;
    #define OS_USEFUL   1;      // 可被操作系统使用的RAM
    #define USED_OR_PR  2;      // 已使用或系统保留，不能使用
    // 其他 未定义
} ARDS;


#endif