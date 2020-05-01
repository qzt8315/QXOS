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

// 4K对齐, 往后对齐
#define ADDR_4K_CEIL(addr)      (((u32)addr+N_4K-1)&(~(N_4K-1)))
// 4K对齐, 往前对齐
#define ADDR_4K_FLOOR(addr)     ((u32)addr&(~(N_4K-1)))

#endif