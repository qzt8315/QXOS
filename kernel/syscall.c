// 用于处理系统调用
#include "const.h"
#include "type.h"
#include "protect.h"
#include "proc.h"
#include "global.h"
#include "proto.h"

PUBLIC  u32 sys_call_handler(u32 sys_no){
    switch (sys_no)
    {
    case 383:
        display_str_colorful("+", 0x0f);
        return ticks;
        break;
    
    default:
        break;
    }
    return 0;
}