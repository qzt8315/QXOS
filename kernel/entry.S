// 汇编语言入口
// 首先移植较为简单的kernel4.asm(系统调用)
#include "syscall.h"
#include "const.h"

.extern    saveregs
.extern    sys_call_handler
.extern    proc_table

.section .text
.code32
.align 4


.global     sys_call_entry

// 内核入口


// 系统中断调用入口
sys_call_entry:
    // 填充对系统调用中无用的errorcode
   pushl   $0
   call    saveregs
   pushl   %eax
   call    sys_call_handler
   add     $4, %esp
   movl    %eax, (proc_table + N_4K + 4*11)
   sti
   ret

.section .bss
.align 4

.global	STACKTOP
.global	ARDS_SPACE
.global	PAGESPACE
.global	_EPAGESPACE
.global	USERSTACKTOP_TEMP

.global	PAGE1DEFAULT
.global	PAGE2DEFAULT
.global	PAGE4DEFAULT
.global	PAGE8DEFAULT
.global	PAGE16DEFAULT
.global	PAGE32DEFAULT
.global	PAGE64DEFAULT
.global	PAGE128DEFAULT
.global	PAGE256DEFAULT
.global	PAGE512DEFAULT
.global	PAGE1024DEFAULT


STACKSPACE:	.skip	STACK_SIZE
STACKTOP:
ARDS_SPACE:	.skip	ARDS_SIZE*ARDS_COUNT
PAGESPACE:	.skip	(VKRLADDRSIZE+VKRLADDRSIZE)/N_4K*PAGEITEMSIZE
_EPAGESPACE:
USERSTACK_TEMP:	   .skip	N_4K
USERSTACKTOP_TEMP:
// 用于管理空余内存
// 保存1个页帧,默认区域,不够后期拓展
PAGE1DEFAULT:        .skip  N_4K

// 保存2个页帧,默认区域,不够后期拓展
PAGE2DEFAULT:        .skip	N_4K

// 保存4个页帧,默认区域,不够后期拓展
PAGE4DEFAULT:		   .skip	N_4K

// 保存8个页帧,默认区域,不够后期拓展
PAGE8DEFAULT:		   .skip	N_4K

// 保存16个页帧,默认区域,不够后期拓展
PAGE16DEFAULT:		   .skip	N_4K

// 保存32个页帧,默认区域,不够后期拓展
PAGE32DEFAULT:		   .skip	N_4K

// 保存64个页帧,默认区域,不够后期拓展
PAGE64DEFAULT:		   .skip  N_4K

// 保存128个页帧,默认区域,不够后期拓展
PAGE128DEFAULT:		.skip	N_4K

// 保存256个页帧,默认区域,不够后期拓展
PAGE256DEFAULT:		.skip	N_4K

// 保存512个页帧,默认区域,不够后期拓展
PAGE512DEFAULT:		.skip	N_4K

// 保存1024个页帧,默认区域,不够后期拓展
PAGE1024DEFAULT:	   .skip	N_4K
