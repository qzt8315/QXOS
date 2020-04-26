; 需要汇编调用的地方

_NR_GET_TICKS equ 0
INT_VECTOR_SYS_CALL equ 0x80
[section .text]

global get_ticks

align 32
[bits 32]
get_ticks:
    mov eax, _NR_GET_TICKS
    int INT_VECTOR_SYS_CALL
    ret
