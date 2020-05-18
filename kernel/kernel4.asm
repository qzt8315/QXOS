; 用于处理系统调用
%include "kernel.inc"
extern  saveregs
extern  sys_call_handler
extern  proc_table

[section .text]
global  sys_call

align   32
[bits   32]
sys_call:
    ; 用于填充error code
    push    0
    call    saveregs
    push    eax
    call    sys_call_handler
    add     esp, 4
    mov     dword [proc_table + _4K + 4*11], eax
    sti
    ret