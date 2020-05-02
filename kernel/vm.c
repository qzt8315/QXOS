// 和虚拟内存有关
// 参考epos实现方式 link:https://github.com/hongmingjian/epos
#include "type.h"
#include "vm.h"
#include "mem.h"
#include "proto.h"
#include "global.h"
#include "krllibc.h"
// 初始化内核虚拟内存
extern void*    _kend;
extern void*    _kstart; 
extern ARDS     ARDS_SPACE;
extern MEMFREEBLOCK MEMFREEBLOCKSPACE;
extern PDE      PAGESPACE;
extern PDE      _EPAGESPACE;

// 函数声明
void    setPDEBaseAddr(PDE* pde, void* addr);
void    setPDEAttr(PDE* pde, u8 attr);
void*   getPDEBaseAddr(PDE* pde);
void    setPTEBaseAddr(PTE* pde, void* addr);
void*   getPTEBaseAddr(PTE* pte);
void    setPTEAttr(PTE* pte, u8 attr);

// 初始化内存管理，并从物理地址切换到虚拟地址运行
void init_vm(){
    // 拷贝ARDS
    ARDS *s_ards = (void*)0xc000; // 由loader中决定
    u8     n = *((u8*)(s_ards+20));
    // MEMFREEBLOCK*  pre_block = NULL;
    int     i;
    *((u8*)V2P(&n_ards)) = n;
    Memcpy(V2P(&ARDS_SPACE), s_ards, n * sizeof(ARDS));
    *((u32*)V2P(&p_ards)) = &ARDS_SPACE;
    s_ards = (ARDS *)V2P(&ARDS_SPACE);
    
    // 查找空闲内存
    // MemFreeStartBlock = NULL;
    MEMFREEBLOCK* p_MemFreeBlock = V2P(&MEMFREEBLOCKSPACE);
    u32* pMemTotal = V2P(&MemTotal);
    u32* pMemFree  = V2P(&MemFree);
    u32 kernel_start = ADDR_4K_FLOOR(V2P(&_kstart));
    u32 kernel_end   = ADDR_4K_CEIL(V2P(&_kend));
    // *pp_MemFreeStartBlock = NULL;
    for(i=0; i<n; i++, s_ards++){
        (*pMemTotal) += s_ards->LengthLow;
        if(s_ards->type == OS_USEFUL){
            (*pMemFree) += s_ards->LengthLow;
            // MEMFREEBLOCK* temp_p;
            u32 free_start = ADDR_4K_CEIL(s_ards->BaseAddrLow);
            u32 free_end   = ADDR_4K_FLOOR(s_ards->BaseAddrLow + s_ards->LengthLow);
            // 32位模式下直接使用低32位即可
            if((kernel_start >= free_end) | (kernel_end <= free_start)){
                // 内核空间与空闲空间完全不重合
                p_MemFreeBlock->baseAddr = s_ards->BaseAddrLow;
                p_MemFreeBlock->length   = s_ards->LengthLow;
                p_MemFreeBlock++;
            }
            else if((kernel_start > free_start)&&(kernel_end >= free_end)){
                p_MemFreeBlock->baseAddr = s_ards->BaseAddrLow;
                p_MemFreeBlock->length   = (u32)V2P(&_kstart) - s_ards->BaseAddrLow;
                p_MemFreeBlock++;
            }
            else if((kernel_start <= free_start)&&(kernel_end < free_end)){
                p_MemFreeBlock->baseAddr = V2P(&_kend);
                p_MemFreeBlock->length   = s_ards->BaseAddrLow + s_ards->LengthLow - (u32)V2P(&_kend);
                p_MemFreeBlock++;
            }
            else if((kernel_start > free_start)&&(kernel_end < free_end)){
                p_MemFreeBlock->baseAddr = s_ards->BaseAddrLow;
                p_MemFreeBlock->length   = (u32)V2P(&_kstart) - s_ards->BaseAddrLow;
                p_MemFreeBlock++;

                p_MemFreeBlock->baseAddr = V2P(&_kend);
                p_MemFreeBlock->length   = s_ards->BaseAddrLow + s_ards->LengthLow - (u32)V2P(&_kend);
                p_MemFreeBlock++;
            }
        }
    }

    // 建立内核空间映射
    // 内核空间4K对齐
    // 虚拟地址
    u32 _4k_vkstart = ADDR_4K_FLOOR(&_kstart);
    u32 _4k_vkend   = ADDR_4K_CEIL(&_kend);
    // 物理地址
    u32 _4k_pkstart = ADDR_4K_FLOOR(V2P(&_kstart));
    u32 _4k_pkend   = ADDR_4K_CEIL(V2P(&_kend));

    // 初始化所有分页
    void*   p_pageStart = ADDR_4K_CEIL(V2P(&PAGESPACE));
    void*   p_pageEnd   = ADDR_4K_FLOOR(V2P(&_EPAGESPACE));
    // 这里记录的是虚拟地址
    FPAGE*  pre_fp      = NULL; 
    // 这里使用物理地址
    FPAGE*  temp_fp     = NULL;
    //
    FPAGE** ph_FreePage = V2P(&pFreePage);
    for(;p_pageStart<p_pageEnd; p_pageStart+=sizeof(PDE) * PAGEITEMS){
        temp_fp = p_pageStart;
        temp_fp->pre = pre_fp;
        temp_fp->next = NULL;
        temp_fp = P2V(temp_fp);
        if(pre_fp == NULL){
            *ph_FreePage = temp_fp;
        }else{
            ((FPAGE*)V2P(pre_fp))->next = temp_fp;
        }
        pre_fp = temp_fp;
    }

    // 开始进行内存映射
    PDE** pPDE = V2P(&pPDETable);
    // 从空余的分页中取出一个作为pde
    *pPDE = V2P(*ph_FreePage);
    *ph_FreePage = (*(FPAGE*)V2P(*ph_FreePage)).next;
    Memset(*pPDE, 0, PAGESIZE);
    // 从空余的分页中取出一个作为pte表
    PTE* pPTE = NULL;
    // 将内核的物理地址完全与虚拟地址对应
    for(;_4k_pkstart<_4k_pkend; _4k_pkstart += PAGESIZE){
        int indexPDE = PDEINDEX(_4k_pkstart);
        int indexPTE = PTEINDEX(_4k_pkstart);
        PDE*    temppPDE = *pPDE + indexPDE;
        if(temppPDE->attr == 0 && temppPDE->avail_baselow4 == 0 && temppPDE == 0){
            pPTE = V2P(*ph_FreePage);
            *ph_FreePage = (*(FPAGE*)V2P(*ph_FreePage)).next;
            Memset(pPTE, 0, PAGESIZE);
            setPDEBaseAddr(temppPDE, pPTE);
            setPDEAttr(temppPDE, P);
        }else{
            pPTE = getPDEBaseAddr(temppPDE);
        }
        setPTEBaseAddr(pPTE, _4k_pkstart);
        setPTEAttr(pPTE, P);
    }
    
    // 将内核的物理地址完全对应到内核空间地址(3G+)
    for(_4k_pkstart = ADDR_4K_FLOOR(V2P(&_kstart));_4k_vkstart<_4k_vkend; _4k_pkstart += PAGESIZE, _4k_vkstart += PAGESIZE){
        int indexPDE = PDEINDEX(_4k_vkstart);
        int indexPTE = PTEINDEX(_4k_vkstart);
        PDE*    temppPDE = *pPDE + indexPDE;
        if(temppPDE->attr == 0 && temppPDE->avail_baselow4 == 0 && temppPDE == 0){
            
            pPTE = V2P(*ph_FreePage);
            *ph_FreePage = (*(FPAGE*)V2P(*ph_FreePage)).next;
            Memset(pPTE, 0, PAGESIZE);
            setPDEBaseAddr(temppPDE, pPTE);
            setPDEAttr(temppPDE, P);
        }else{
            pPTE = getPDEBaseAddr(temppPDE);
        }
        setPTEBaseAddr(pPTE, _4k_pkstart);
        setPDEAttr(pPTE, P);
    }
}

// 设置PDE基地址
void    setPDEBaseAddr(PDE* pde, void* addr){
    u32* p = (u32*)pde;
    *p     = (ADDR_4K_FLOOR(*p) ^ (*p)) | ADDR_4K_FLOOR(addr);
}

void    setPDEAttr(PDE* pde, u8 attr){
    pde->attr |= attr;
}

void*   getPDEBaseAddr(PDE* pde){
    return (void*)ADDR_4K_FLOOR(*(u32*)pde);
}

// 设置PTE基地址
void    setPTEBaseAddr(PTE* pte, void* addr){
    u32* p = (u32*)pte;
    *p     = (ADDR_4K_FLOOR(*p) ^ (*p)) | ADDR_4K_FLOOR(addr);
}

void*   getPTEBaseAddr(PTE* pte){
    return (void*)ADDR_4K_FLOOR(*(u32*)pte);
}

void    setPTEAttr(PTE* pte, u8 attr){
    pte->attr |= attr;
}