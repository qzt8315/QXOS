// 汇编语言入口
// 首先移植较为简单的kernel4.asm(系统调用)
 #include "syscall.h"
 #include "const.h"

 .extern    saveregs
 .extern    sys_call_handler
 .extern    proc_table

 .text
 .code32
 .globl     sys_call_entry
 .align 4
 sys_call_entry:
    // 填充对系统调用中无用的errorcode
   pushl   $0
   call    saveregs
   pushl   %eax
   call    sys_call_handler
   add     $4, %esp
   movl    %eax, (proc_table + 0x1000 + 4*11)
   sti
   ret
    

 
 