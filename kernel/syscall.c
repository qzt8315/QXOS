// 用于处理系统调用
#include "const.h"
#include "type.h"
#include "protect.h"
#include "proc.h"
#include "global.h"
#include "proto.h"
#include "syscall.h"

PRIVATE u32 sys_call_get_ticks(); 

PUBLIC  u32 sys_call_handler(u32 sys_no){
    switch (sys_no)
    {
    case __INT_GET_TICKS:
        display_str_colorful("+", 0x0f);
        return sys_call_get_ticks();
        break;
    
    default:
        break;
    }
    return 0;
}


u32 sys_call_get_ticks(){
    return ticks;
    // return 1;
}