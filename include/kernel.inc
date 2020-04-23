; 堆栈大小
STACK_SIZE	equ	1024

; 进程表中返回地址的偏移
RETADDR_OFF equ 48

; TSS选择子
TSS_SELECTOR    equ 0x20

; ldt位置
PROC_LDT_OFF    equ 72

; 8259A中断处理
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