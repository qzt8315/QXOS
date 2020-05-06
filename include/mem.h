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
    #define OS_USEFUL   1       // 可被操作系统使用的RAM
    #define USED_OR_PR  2       // 已使用或系统保留，不能使用
    // 其他 未定义
} ARDS;

#define ARDS_SIZE   20

// 用于管理空闲内存， 方法来自于buddy
typedef struct s_buddyblock{
    u16     num;    // 最大数量1022
    #define BUDDYBLOCKMAXNUM    1022
    u16     type;
    #define PAGE1       0       // 每个地址表示1页物理空间
    #define PAGE2       1       // 每个地支表示2页物理空间
    #define PAGE4       2       // 每个地支表示4页物理空间
    #define PAGE8       3       // 每个地支表示8页物理空间
    #define PAGE16      4       // 每个地支表示16页物理空间
    #define PAGE32      5       // 每个地支表示32页物理空间
    #define PAGE64      6       // 每个地支表示64页物理空间
    #define PAGE128     7       // 每个地支表示128页物理空间
    #define PAGE256     8       // 每个地支表示256页物理空间
    #define PAGE512     9       // 每个地支表示512页物理空间
    #define PAGE1024    10      // 每个地支表示1024页物理空间
    void*   pagesAddrs[1021];
    struct s_buddyblock* preBlock;      // 前一个block
    struct s_buddyblock* nextBlock;     // 后一个block
} BUDDYBLOCK;

#endif