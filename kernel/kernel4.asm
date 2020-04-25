; 用于处理系统调用
extern  saveregs
extern  sys_call_handler

[section .text]
global  sys_call

align   32
[bits   32]
sys_call:
    call    saveregs
    push    eax
    call    sys_call_handler
    add     esp, 4
    sti
    ret