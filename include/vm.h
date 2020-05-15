// 用于虚拟内存建设

#ifndef __VM_H__
#define __VM_H__

// 内核实际加载基址 1M
#define K_P_BASE_ADDR       0x100000
// 内核空间虚拟地址基址 3G
#define K_V_BASE_ADDR       0xc0000000

// 4K
#define N_4K                0x1000

// 虚拟地址转物理地址
#define V2P(addr)       ((void*)addr+K_P_BASE_ADDR-K_V_BASE_ADDR)

// 物理地址转虚拟地址
#define P2V(addr)       ((void*)(addr)+K_V_BASE_ADDR-K_P_BASE_ADDR)

// 4K对齐, 往后对齐
#define ADDR_4K_CEIL(addr)      (((u32)(addr)+N_4K-1)&(~(N_4K-1)))
// 4K对齐, 往前对齐
#define ADDR_4K_FLOOR(addr)     ((u32)(addr)&(~(N_4K-1)))

typedef struct s_pde
{
    u8  attr;
    #define P               1   // 在内存中存在
    #define WR              2   // 可读写
    #define USER            4   // 用户级
    #define WRITETHROUGH    8   // 缓冲策略，默认为write-back, 该项为write-through
    #define NOCACHE         16  // 不可缓冲
    u8  avail_baselow4;
    u16 basehigh16;
} PDE;

typedef struct s_pte
{
    u8  attr;
    u8  avail_baselow4;
    u16 basehigh16;
} PTE;

// 记录空闲的页表
typedef struct s_fpage
{
    struct s_fpage* pre;
    struct s_fpage* next;        
} FPAGE;


// 每页数量
#define PAGEITEMS   1024

// 每页大小
#define PAGESIZE    N_4K

// 获取PDE表的index
#define PDEINDEX(addr)  (((u32)(addr) & 0xffc00000)>>22)
// 获取PTE表的index
#define PTEINDEX(addr)  (((u32)(addr) & 0x003ff000)>>12)

//  分配内存空间
#define SMALLOC_TYPE    0
#define KMALLOC_TYPE    1
#define VMALLOC_TYPE    2

// 分配内核地址
void* malloc(u32 size, u8 type);


#endif