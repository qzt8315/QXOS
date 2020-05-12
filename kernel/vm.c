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
extern PDE      PAGESPACE;
extern PDE      _EPAGESPACE;
extern u16      _VCGAMEM;
extern u16      _EVCGAMEM;
extern u8       HIGHMEM;
extern u8       VMALLOCSPACE;
extern u8       _EVMALLOCSPACE;
extern u8       DYNAMICADDR;
extern u8       _EDYNAMICADDR;
extern u8       KERNEL_PAGE_POS;
extern BUDDYBLOCK   PAGE1DEFAULT;
extern BUDDYBLOCK   PAGE2DEFAULT;
extern BUDDYBLOCK   PAGE4DEFAULT;
extern BUDDYBLOCK   PAGE8DEFAULT;
extern BUDDYBLOCK   PAGE16DEFAULT;
extern BUDDYBLOCK   PAGE32DEFAULT;
extern BUDDYBLOCK   PAGE64DEFAULT;
extern BUDDYBLOCK   PAGE128DEFAULT;
extern BUDDYBLOCK   PAGE256DEFAULT;
extern BUDDYBLOCK   PAGE512DEFAULT;
extern BUDDYBLOCK   PAGE1024DEFAULT;

// 函数声明
void    setPDEBaseAddr(PDE* pde, void* addr);
void    setPDEAttr(PDE* pde, u8 attr);
void*   getPDEBaseAddr(PDE* pde);
void    setPTEBaseAddr(PTE* pde, void* addr);
void*   getPTEBaseAddr(PTE* pte);
void    setPTEAttr(PTE* pte, u8 attr);
void*   getFreePage();
void    freePage(void* pPage);
void*   recordInBuddyBlock(void* startAddr, u32   length, int deep, u8 type);
void    initBuddyBlocks();
void*   getFreeKVAddr(u32   size, u8 type);
void*   getFreeMem(u32  size);
int     mmap(void* PAddr, void* VAddr, u8 attr);

// 初始化内存管理，并从物理地址切换到虚拟地址运行
void init_vm(){
    // 将原本的GDT复制到新位置并初始化GDT
	Memcpy(V2P(gdt),
		(void*)(*((u32*)(&((u8*)V2P(gdt_ptr))[2]))),
		*((u16*)V2P(gdt_ptr)) + 1
		);
	u16 *ptr_limit = (u16*)V2P(gdt_ptr);
	u32 *ptr_base  = (u32*)(&((u8*)V2P(gdt_ptr))[2]);
	*ptr_limit = GDT_SIZE * sizeof(DESCRIPTOR) - 1;
	*ptr_base  = (u32)gdt;


    // 拷贝ARDS
    ARDS *s_ards = (void*)0xc000; // 由loader中决定
    u8     n = *((u8*)(s_ards+20));
    // MEMFREEBLOCK*  pre_block = NULL;
    int     i;
    *((u8*)V2P(&n_ards)) = n;
    Memcpy(V2P(&ARDS_SPACE), s_ards, n * sizeof(ARDS));
    *((ARDS**)V2P(&p_ards)) = &ARDS_SPACE;
    s_ards = (ARDS *)V2P(&ARDS_SPACE);
    
    // 查找空闲内存
    // 初始化buddyBLOCK
    initBuddyBlocks();

    // MemFreeStartBlock = NULL;
    u32* pMemTotal = V2P(&MemTotal);
    u32* pMemFree  = V2P(&MemFree);
    u32 kernel_start = ADDR_4K_FLOOR(V2P(&_kstart));
    u32 kernel_end   = ADDR_4K_CEIL(V2P(&_kend));
    for(i=0; i<n; i++, s_ards++){
        (*pMemTotal) += s_ards->LengthLow;
        if(s_ards->type == OS_USEFUL){
            (*pMemFree) += s_ards->LengthLow;
            u32 free_start = ADDR_4K_CEIL(s_ards->BaseAddrLow);
            u32 free_end   = ADDR_4K_FLOOR(s_ards->BaseAddrLow + s_ards->LengthLow);
            // 32位模式下直接使用低32位即可
            if((kernel_start >= free_end) | (kernel_end <= free_start)){
                // 内核空间与空闲空间完全不重合
                recordInBuddyBlock((void*)(s_ards->BaseAddrLow), s_ards->LengthLow, 0, 0);
            }
            else if((kernel_start > free_start)&&(kernel_end >= free_end)){
                recordInBuddyBlock((void*)(s_ards->BaseAddrLow), (u32)V2P(&_kstart) - s_ards->BaseAddrLow, 0, 0);
            }
            else if((kernel_start <= free_start)&&(kernel_end < free_end)){
                recordInBuddyBlock(V2P(&_kend), s_ards->BaseAddrLow + s_ards->LengthLow - (u32)V2P(&_kend), 0, 0);
            }
            else if((kernel_start > free_start)&&(kernel_end < free_end)){
                recordInBuddyBlock((void*)(s_ards->BaseAddrLow), (u32)V2P(&_kstart) - s_ards->BaseAddrLow, 0, 0);

                recordInBuddyBlock(V2P(&_kend), s_ards->BaseAddrLow + s_ards->LengthLow - (u32)V2P(&_kend), 0, 0);
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
    void*   p_pageStart = (void*)ADDR_4K_CEIL(V2P(&PAGESPACE));
    void*   p_pageEnd   = (void*)ADDR_4K_FLOOR(V2P(&_EPAGESPACE));
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
    PTE* pPTE = NULL;
    // 将内核的物理地址完全与虚拟地址对应
    for(;_4k_pkstart<_4k_pkend; _4k_pkstart += PAGESIZE){
        int indexPDE = PDEINDEX(_4k_pkstart);
        int indexPTE = PTEINDEX(_4k_pkstart);
        PDE*    temppPDE = *pPDE + indexPDE;
        if(temppPDE->attr == 0 && temppPDE->avail_baselow4 == 0 && temppPDE->basehigh16 == 0){
            pPTE = V2P(*ph_FreePage);
            *ph_FreePage = (*(FPAGE*)V2P(*ph_FreePage)).next;
            Memset(pPTE, 0, PAGESIZE);
            setPDEBaseAddr(temppPDE, pPTE);
            setPDEAttr(temppPDE, P | WR);
        }else{
            pPTE = getPDEBaseAddr(temppPDE);
        }
        pPTE += indexPTE;
        setPTEBaseAddr(pPTE, (void*)_4k_pkstart);
        setPTEAttr(pPTE, P | WR);
    }
    
    // 将内核的物理地址完全对应到内核空间地址(3G+)
    for(_4k_pkstart = ADDR_4K_FLOOR(V2P(&_kstart));_4k_vkstart<_4k_vkend; _4k_pkstart += PAGESIZE, _4k_vkstart += PAGESIZE){
        int indexPDE = PDEINDEX(_4k_vkstart);
        int indexPTE = PTEINDEX(_4k_vkstart);
        PDE*    temppPDE = *pPDE + indexPDE;
        if(temppPDE->attr == 0 && temppPDE->avail_baselow4 == 0 && temppPDE->basehigh16 == 0){
            
            pPTE = V2P(*ph_FreePage);
            *ph_FreePage = (*(FPAGE*)V2P(*ph_FreePage)).next;
            Memset(pPTE, 0, PAGESIZE);
            setPDEBaseAddr(temppPDE, pPTE);
            setPDEAttr(temppPDE, P | WR);
        }else{
            pPTE = getPDEBaseAddr(temppPDE);
        }
        pPTE += indexPTE;
        setPTEBaseAddr(pPTE, (void*)_4k_pkstart);
        setPTEAttr(pPTE, P | WR);
    }

    // 映射显存
    void* p_vcga = (void*)&_VCGAMEM;
    void* p_vcgaend = (void*)&_EVCGAMEM;
    void* p_pcga = (void*)PCGAMEM;
    *(u16**)V2P(&pVGAMEM) = p_vcga;
    for(; p_vcga<p_vcgaend; p_vcga+=PAGESIZE, p_pcga+=PAGESIZE){
        int indexPDE = PDEINDEX(p_vcga);
        int indexPTE = PTEINDEX(p_vcga);
        PDE*    temppPDE = *pPDE + indexPDE;
        if(temppPDE->attr == 0 && temppPDE->avail_baselow4 == 0 && temppPDE->basehigh16 == 0){
            
            pPTE = V2P(*ph_FreePage);
            *ph_FreePage = (*(FPAGE*)V2P(*ph_FreePage)).next;
            Memset(pPTE, 0, PAGESIZE);
            setPDEBaseAddr(temppPDE, pPTE);
            setPDEAttr(temppPDE, P | WR);
        }else{
            pPTE = getPDEBaseAddr(temppPDE);
        }
        pPTE += indexPTE;
        setPTEBaseAddr(pPTE, (void*)p_pcga);
        setPTEAttr(pPTE, P | WR);
    }

    // 将分页信息映射到指定位置
    int iPDE    = PDEINDEX(&KERNEL_PAGE_POS);
    PDE* kpPage = *pPDE + iPDE;
    setPDEBaseAddr(kpPage, (void*)*pPDE);
    setPDEAttr(kpPage, P| WR);
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

// 取消内核物理地址映射
void    kernelUnMap(){
    // dis_pos = 0;
    // char*   s = "123";
    // display_str_colorful(s, 0x0f);
    void*   _4k_pstart  = (void *)ADDR_4K_FLOOR(V2P(&_kstart));
    void*   _4k_pend    = (void *)ADDR_4K_FLOOR(V2P(&_kend));
    for(; _4k_pstart<_4k_pend; _4k_pstart+= PAGEITEMS * PAGESIZE){
        int nPdeIndex = PDEINDEX(_4k_pstart);
        PDE* pPde = pPDETable+nPdeIndex;
        PTE* pPte = getPDEBaseAddr(pPde);
        pPde->attr = 0;
        pPde->avail_baselow4 = 0;
        pPde->basehigh16 = 0;
        freePage((void*)P2V(pPte));
    }
}

//获取一个空闲页
void*   getFreePage(){
    if(pFreePage == NULL);
        return NULL;
    FPAGE* ret = pFreePage;
    pFreePage = pFreePage->next;
    if(pFreePage != NULL)
        pFreePage->pre = NULL;
    return ret;
}
// 释放页
void    freePage(void* pPage){
    if(pPage == NULL)
        return;
    FPAGE* p = (FPAGE*)pPage;
    if(pFreePage != NULL){
        pFreePage->pre = p;
    }
    p->pre = NULL;
    p->next = pFreePage;
    pFreePage = p;
}

// 初始化BuddyBlock
void    initBuddyBlocks(){
    void*   addrList[]      = {&PAGE1DEFAULT, &PAGE2DEFAULT,
                                &PAGE4DEFAULT, &PAGE8DEFAULT,
                                &PAGE16DEFAULT, &PAGE32DEFAULT,
                                &PAGE64DEFAULT, &PAGE128DEFAULT,
                                &PAGE256DEFAULT, &PAGE512DEFAULT,
                                &PAGE1024DEFAULT};
    BUDDYBLOCK* temp_pBB    = NULL;
    int i;

    // 初始化buddyblock
    for(i =0 ; i<sizeof(addrList)/sizeof(addrList[0]); i++){
        *(BUDDYBLOCK**)V2P(&pBuddyBlocks[i])    = addrList[i];
        temp_pBB                                = V2P(addrList[i]);
        temp_pBB->num                           = 0;
        temp_pBB->type                          = (u16)i;
        temp_pBB->preBlock                      = NULL;
        temp_pBB->nextBlock                     = NULL;
    }
    
}

// 将一段物理内存放入空闲内存中以便可以分配内存
// 返回的内容是调整后的起始地址, 起初调用的deep为0
// type = 0:运行在物理地址
// type = 1:运行在虚拟地址
void*   recordInBuddyBlock(void* startAddr, u32   length, int deep, u8 type){
    u32 maxDeep;
    if(type){
        maxDeep = sizeof(pBuddyBlocks)/sizeof(pBuddyBlocks[0])-1;
    }else{
        maxDeep = sizeof(pBuddyBlocks)/sizeof(((BUDDYBLOCK**)V2P(pBuddyBlocks))[0])-1;
    }
    if(deep<0 || deep > maxDeep)
        return startAddr;
    u32 minL    = N_4K * (1<<deep);
    void*   endAddr = startAddr+length;
    startAddr       = (void*)ADDR_4K_CEIL(startAddr);
    endAddr         = (void*)ADDR_4K_FLOOR(endAddr);
    length          = endAddr - startAddr;
    if(length < minL)
        return startAddr;
    else if(length >= (minL<<1) && deep < maxDeep){
        startAddr = recordInBuddyBlock(startAddr, length, deep+1, type);
        length = endAddr - startAddr;
    }
    do{
        BUDDYBLOCK* temp_pBuddyBlock;
        if(type){
            temp_pBuddyBlock = *(pBuddyBlocks+deep);
        }else{
            temp_pBuddyBlock = V2P(*((BUDDYBLOCK**)V2P(pBuddyBlocks)+deep));
        }
        temp_pBuddyBlock->pagesAddrs[temp_pBuddyBlock->num] = startAddr;
        startAddr += minL;
        length = endAddr-startAddr;
    }while(length>minL);
    return startAddr;
}

// 申请分配内核空间,分配3G+896M之间的地址
// 当申请成功时返回虚拟地址，当申请失败时返回空指针
// 申请大小单位为4K
// type定义:    
// 0: 不保证物理地址连续，并且在896M以内,优先物理地址连续
// 1: 保证物理地址连续性，并且在896M以内
// 2: 不保证物理地址连续性，并且在896M以外
void* malloc(u32 size, u8 type){
    u32 _4k_size = ADDR_4K_CEIL(size);
    void* pVaddr = NULL;
    void* ret    = pVaddr;
    void* pPaddr = NULL;
    u32   mCount = 0;
    switch (type)
    {
    case SMALLOC_TYPE:
        pVaddr = getFreeKVAddr(_4k_size, 0);
        ret = pVaddr;
        do{
            void* pPaddr= getFreeMem(N_4K);
            if(pPaddr == (void*)0xffffffff)
                return NULL;
            else
                mmap(pPaddr, pVaddr, P | WR);
            
            mCount += N_4K;
            pVaddr += N_4K;
        }while(mCount < _4k_size);
        return  ret;
        break;
    case KMALLOC_TYPE:
        break;
    case VMALLOC_TYPE:
        break;
    default:
        return  NULL;
        break;
    }
}

// 获取连续虚拟地址
// type=0: 3G~3G+896M
// type=1: vmalloc区域
// type=2: 动态映射区
void*   getFreeKVAddr(u32   size, u8 type){
    const u32   uPage       = ADDR_4K_CEIL(size)>>12;
    u32         uCount      = 0;
    void*       pStart;
    void*       pEnd;
    switch (type)
    {
    case 0:
        pStart  = (void*)ADDR_4K_CEIL(&_kend);
        pEnd    = (void*)ADDR_4K_FLOOR(&HIGHMEM);
        break;

    case 1:
        pStart  = (void*)ADDR_4K_CEIL(&VMALLOCSPACE);
        pEnd    = (void*)ADDR_4K_FLOOR(&_EVMALLOCSPACE);
        break;

    case 2:
        pStart  = (void*)ADDR_4K_CEIL(&DYNAMICADDR);
        pEnd    = (void*)&_EDYNAMICADDR;
        break;
    
    default:
        return  NULL;
        break;
    }
    void* pVAddr = pStart;
    while(uCount< uPage || pVAddr<pEnd){
        u32 iPDE = PDEINDEX(pVAddr);
        u32 iPTE = PTEINDEX(pVAddr);
        PDE*    pPdePage = (PDE*)(((u32)&KERNEL_PAGE_POS)|(((u32)&KERNEL_PAGE_POS)>>10))+iPDE;
        PTE*    pPtePage = (PTE*)getPDEBaseAddr(pPdePage);
        if(pPdePage == NULL && ((PTE*)P2V(pPdePage))->attr == NULL){
            uCount++;
            pVAddr+=N_4K;
        }else{
            // 地址指定的PTE
            PTE*    pPte = (PTE*)P2V(pPdePage)+iPTE;
            if(getPTEBaseAddr(pPte) == NULL && pPte->attr == NULL){
                uCount++;
                pVAddr+=N_4K;
            }else{
                uCount=0;
                pVAddr+=N_4K;
                pStart = pVAddr;
            }
        }
    }

    if(uCount>=uPage){
        return pStart;
    }else{
        return NULL;
    }
}

// 获取空闲的物理内存(连续的)
// 最大支持4M(1024页)，最小分割单位4K，当返回0xffffffff时地址无效
void*   getFreeMem(u32  size){
    if(size == 0 || size >= 0x400000)
        return (void*)0xffffffff;
    u32 _4k_size = ADDR_4K_CEIL(size);
    u8  u8count  = 1;
    void*   retAddr = (void*)0xffffffff;
    while((_4k_size>>u8count) ==1){
        u8count++;
    }
    u32 _2nSize = _4k_size > (1<<u8count)? 1<<(++u8count) : 1<<u8count ;
    u8  ibuddyblocks = u8count - 13;
    if(pBuddyBlocks[ibuddyblocks]->num == 0){
        retAddr = getFreeMem(_2nSize*2);
        if(retAddr != (void*)0xffffffff){
            u16 index = pBuddyBlocks[ibuddyblocks]->num;
            if(index < BUDDYBLOCKMAXNUM){
                pBuddyBlocks[ibuddyblocks]->pagesAddrs[index] = retAddr+_2nSize;
                pBuddyBlocks[ibuddyblocks]->num+=1;
            }else{
                void*   pVaddr = getFreeKVAddr(N_4K, 0);
                void*   pPaddr = getFreeMem(N_4K);

            }
        }
    }else{
        u16 index = pBuddyBlocks[ibuddyblocks]->num - 1;
        if(index >= BUDDYBLOCKMAXNUM)
            return (void*)0xffffffff;
        retAddr = pBuddyBlocks[ibuddyblocks]->pagesAddrs[index];
        pBuddyBlocks[ibuddyblocks]->num -= 1;
    }
}

//  对应一个4K页
int mmap(void* PAddr, void* VAddr, u8 attr){
    PDE*    pPdePage = (PDE*)((u32)&KERNEL_PAGE_POS | ((u32)&KERNEL_PAGE_POS)>>10);
    u32     iPDE     = PDEINDEX(VAddr);
    u32     iPTE     = PTEINDEX(VAddr);
    void*   pPTE     = getPDEBaseAddr(pPdePage+iPDE);
    if(pPTE != NULL && !(((pPdePage+iPDE)->attr)&1)){
        PTE*    pPTE = (PTE*)((u32)&KERNEL_PAGE_POS | iPDE << 12) + iPTE;
        setPTEBaseAddr(pPTE, PAddr);
        setPTEAttr(pPTE, attr);
    }else{
        void*   freePage = getFreePage();
        setPDEBaseAddr(pPdePage+iPDE, freePage);
        setPDEAttr(pPdePage+iPDE, attr);
        PTE*    pPTE = (PTE*)((u32)&KERNEL_PAGE_POS | iPDE << 12) + iPTE;
        setPTEBaseAddr(pPTE, PAddr);
        setPTEAttr(pPTE, attr);
    }
    return 0;
}