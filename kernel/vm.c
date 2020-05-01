// 和虚拟内存有关
// 参考epos实现方式 link:https://github.com/hongmingjian/epos
#include "type.h"
#include "vm.h"
#include "mem.h"
#include "proto.h"
#include "global.h"
// 初始化内核虚拟内存
extern void*  _kend;
extern void*  _kstart; 
extern ARDS    ARDS_SPACE;

// 初始化内存管理，并从物理地址切换到虚拟地址运行
void init_vm(){
    // 拷贝ARDS
    ARDS *s_ards = (void*)0xc000; // 由loader中决定
    u8     n = *((u8*)(s_ards+20));
    MEMFREEBLOCK*  pre_block = NULL;
    int     i;
    *((u8*)V2P(&n_ards)) = n;
    Memcpy(V2P(&ARDS_SPACE), s_ards, n * sizeof(ARDS));
    *((u32*)V2P(&p_ards)) = &ARDS_SPACE;
    s_ards = (ARDS *)V2P(&ARDS_SPACE);
    
    // 查找空闲内存
    // MemFreeStartBlock = NULL;
    MEMFREEBLOCK** pp_MemFreeStartBlock = V2P(&MemFreeStartBlock);
    u32* pMemTotal = V2P(&MemTotal);
    u32* pMemFree  = V2P(&MemFree);
    u32 kernel_start = ADDR_4K_FLOOR(V2P(&_kstart));
    u32 kernel_end   = ADDR_4K_CEIL(V2P(&_kend));
    *pp_MemFreeStartBlock = NULL;
    for(i=0; i<n; i++, s_ards++){
        (*pMemTotal) += s_ards->LengthLow;
        if(s_ards->type == OS_USEFUL){
            (*pMemFree) += s_ards->LengthLow;
            MEMFREEBLOCK* temp_p;
            u32 free_start = ADDR_4K_CEIL(s_ards->BaseAddrLow);
            u32 free_end   = ADDR_4K_FLOOR(s_ards->BaseAddrLow + s_ards->LengthLow);
            // 32位模式下直接使用低32位即可
            if((kernel_start >= free_end) | (kernel_end <= free_start)){
                // 内核空间与空闲空间完全不重合
                temp_p = (MEMFREEBLOCK*)(s_ards->BaseAddrLow);
                temp_p->pre = pre_block;
                temp_p->next = NULL;
                temp_p->length= s_ards->LengthLow;
                if(*pp_MemFreeStartBlock == NULL && pre_block == NULL){
                    *pp_MemFreeStartBlock = temp_p;
                }else{
                    pre_block->next = temp_p;
                }
                pre_block = temp_p;
            }
            else if((kernel_start > free_start)&&(kernel_end >= free_end)){
                temp_p = (MEMFREEBLOCK*)(s_ards->BaseAddrLow);
                temp_p->pre  = pre_block;
                temp_p->next = NULL;
                temp_p->length = (u32)V2P(&_kstart) - s_ards->BaseAddrLow;

                if(*pp_MemFreeStartBlock == NULL && pre_block == NULL){
                    *pp_MemFreeStartBlock = temp_p;
                }else{
                    pre_block->next = temp_p;
                }
                pre_block = temp_p;
            }
            else if((kernel_start <= free_start)&&(kernel_end < free_end)){
                temp_p = (MEMFREEBLOCK*)V2P(&_kend);
                temp_p->pre  = pre_block;
                temp_p->next = NULL;
                temp_p->length = s_ards->BaseAddrLow + s_ards->LengthLow - (u32)V2P(&_kend);

                if(*pp_MemFreeStartBlock == NULL && pre_block == NULL){
                    *pp_MemFreeStartBlock = temp_p;
                }else{
                    pre_block->next = temp_p;
                }
                pre_block = temp_p;
            }
            else if((kernel_start > free_start)&&(kernel_end < free_end)){
                temp_p = (MEMFREEBLOCK*)(s_ards->BaseAddrLow);
                temp_p->pre  = pre_block;
                temp_p->next = NULL;
                temp_p->length = (u32)V2P(&_kstart) - s_ards->BaseAddrLow;

                if(*pp_MemFreeStartBlock == NULL && pre_block == NULL){
                    *pp_MemFreeStartBlock = temp_p;
                }else{
                    pre_block->next = temp_p;
                }
                pre_block = temp_p;


                temp_p = (MEMFREEBLOCK*)V2P(&_kend);
                temp_p->pre  = pre_block;
                temp_p->next = NULL;
                temp_p->length = s_ards->BaseAddrLow + s_ards->LengthLow - (u32)V2P(&_kend);
                pre_block->next = temp_p;
                pre_block = temp_p;
            }
        }
    }

}