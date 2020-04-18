; 堆栈大小
STACK_SIZE	equ	1024

; 进程表中返回地址的偏移
RETADDR_OFF equ 48


; 8259A中断处理
%macro  irq_master  1
    call    saveregs
    push    %1
    call    disableIRQ
    call    sendEOI2Master
    call    hwint_handler
    call    enableIRQ
%endmacro

%macro  irq_slave   1
    call    saveregs
    push    %1
    call    disableIRQ
    call    sendEOI2Slave
    call    hwint_handler
    call    enableIRQ
%endmacro