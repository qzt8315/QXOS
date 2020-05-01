// 和虚拟内存有关
// 参考epos实现方式 link:https://github.com/hongmingjian/epos
#include "type.h"
#include "vm.h"
#include "mem.h"
#include "proto.h"
#include "global.h"
// 初始化内核虚拟内存
extern void*  _end;
extern ARDS    ARDS_SPACE;

// 目前存在的问题，如何确认内核代码所占用的内存
void init_vm(){
    ARDS *s_ards = (void*)0xc000; // 由loader中决定
    u8     n = *((u8*)(s_ards+20));
    *((u8*)V2P(&n_ards)) = n;
    Memcpy(V2P((void*)&ARDS_SPACE), s_ards, n * sizeof(ARDS));
    *((u32*)V2P((void*)&p_ards)) = &ARDS_SPACE;
}