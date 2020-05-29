// 内核中需要汇编实现的函数
#include    "const.h"


.extern     dis_pos
.extern     pVGAMEM
.extern     proc_table
.extern     cur_proc
.extern     nRenter
.extern     n_timeSlice
.extern     b_SwitchProc


.extern     set_TSS_stack_ring0

#define	PER_CHAR_LEN	2
#define	LINE_CHARS	    80
#define	LINES		    25
#define  RETADDR_OFF    48

.section    .text
.code32
.align  4

.global     out_byte
.global     in_byte
.global     saveregs
.global     restart
.global     load_tss
.global     load_ldt
.global     context_switch

// 向端口中写入字节
// void out_byte(u16 port, u8 b_byte)
out_byte:
    pushl   %edx
    pushl   %eax
    movl    12(%esp), %edx
    movb    16(%esp), %al
    outb    %al, %dx
    nop
    nop
    popl    %eax
    popl    %edx
    ret

// 从端口中读取字节
// u8 in_byte(u16 port)
in_byte:
    pushl   %edx
    movl    8(%esp), %edx
    inb     %dx, %al
    nop
    nop
    popl    %edx
    ret

// 保存寄存器的值
saveregs:
    pushal
    push	%ds
    push	%es
    push	%fs
    push	%gs

    movw  %ss, %bx
    movw  %bx, %ds
    movw  %bx, %es
    movl  %esp, %ebp

    jmp   *RETADDR_OFF(%ebp)

// 切换进程,不会直接调用，由restart决定是否切换进程
// void context_switch(PROCESS* nextProc);
context_switch:
    movl    $0, (b_SwitchProc)
    movl    $TIMESLICE, (n_timeSlice)
    movl    4(%esp), %eax
    addl    $N_4K, %eax
    movl    %eax, %esp
    addl    $76, %eax
    pushl   %eax
    pushl   $0x10
    call    set_TSS_stack_ring0
    addl    $8, %esp
    call    restart


// 恢复进程
// void restart();
restart:
    cli
    cmpl    $0, (b_SwitchProc)
    je      .1
    pushl   (cur_proc)
    call    context_switch
.1:
    addl    $4, %esp
    pop     %gs
    pop     %fs
    pop     %es
    pop     %ds
    popal
    addl    $8, %esp
    decl    (nRenter)
    sti
    iret

// 加载tss
//  void    load_tss(u16    tss_sel);
load_tss:
    ltr   4(%esp)
    ret

// 加载ldt
// void     load_ldt(u16    ldt_sel);
load_ldt:
    lldt  4(%esp)
    ret