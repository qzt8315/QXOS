; 堆栈大小
STACK_SIZE	equ	1024

; 进程表中返回地址的偏移
RETADDR_OFF equ 48

; TSS选择子
TSS_SELECTOR    equ 0x20

; ldt位置
PROC_LDT_OFF    equ 72

; 8259A中断处理
; 8259从片中断处理
%macro  irq_master  1
    call    saveregs
    push    %1
    call    disableIRQ
    call    sendEOI2Master
    sti
    call    hwint_handler
    call    enableIRQ
    add     esp, 4
    ret
%endmacro

; 8259从片中断处理
%macro  irq_slave   1
    call    saveregs
    push    %1
    call    disableIRQ
    call    sendEOI2Slave
    sti
    call    hwint_handler
    call    enableIRQ
    add     esp, 4
    ret
%endmacro


; 内核实际加载基址 1M
K_P_BASE_ADDR	equ	0x100000
; 内核空间虚拟地址基址 3G
K_V_BASE_ADDR	equ	0xc0000000


; 虚拟地址转物理地址
; %macro V2P  1
;     %1+K_P_BASE_ADDR-K_V_BASE_ADDR
; %endmacro
%define V2P(vaddr)   vaddr+K_P_BASE_ADDR-K_V_BASE_ADDR