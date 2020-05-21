// 汇编语言入口
// 首先移植较为简单的kernel4.asm(系统调用)
// 已移植kernel.asm
// 已移植kernel2.asm
#include "syscall.h"
#include "const.h"

// 汇编中使用到的宏
#define  V2P_(vaddr)   (vaddr+K_P_BASE_ADDR-K_V_BASE_ADDR)
#define  TSS_SELECTOR   0x20
#define  exception_with_errorcode(ex_code)   \
   call     saveregs;      \
   movl     %esp, %ebp;    \
   pushl    68(%ebp);      \
   pushl    64(%ebp);      \
   pushl    60(%ebp);      \
   pushl    56(%ebp);      \
   pushl    $ex_code;      \
   jmp      exception;

#define  exception_no_errorcode(ex_code)     \
   pushl    $0xffffffff;               \
   exception_with_errorcode(ex_code);

.extern     gdt_ptr
.extern     idt_ptr

.extern     saveregs
.extern     sys_call_handler
.extern     proc_table
.extern	   init_vm
.extern     kernelUnMap
.extern     cstart
.extern     init_proc
.extern     restart

.section .text
.code32
.align 4


.global     sys_call_entry
.global     _start
// --------------------------
// 内核入口
// --------------------------
_start:
   movl  $V2P_(STACKTOP), %esp
   sgdt  V2P_(gdt_ptr)
   call  init_vm
   movl  V2P_(pPDETable), %eax
   orl   $8, %eax
   movl  %eax, %cr3
   lgdt  (gdt_ptr)
   // 切换堆栈
   movl  $STACKTOP, %esp
   ljmp  $8,$te
te:
   call  kernelUnMap
   call  cstart
   lidt  (idt_ptr)

   // 创建新进程
   call  init_proc
   pushl $TSS_SELECTOR
   call  load_tss
   addl  $4, %esp
   movw  $0x28, %ax
   lldt  %ax
   movl  $(proc_table+N_4K), %esp
   jmp   restart

load_tss:
   ltr   4(%esp)
   ret

load_ldt:
   lldt  4(%esp)
   ret



// ------------------------
// 异常处理入口
// ------------------------
.global	divide_error
.global	debug_exception
.global	not_mask_interrupt
.global	bug_point
.global	overflow
.global	bounds
.global	invalid_opcode
.global	copr_not_available
.global	double_fault
.global	copr_seg_overrun
.global	invalid_tss
.global	segment_not_present
.global	stack_excption
.global	general_protection
.global	page_fault
.global	copr_error

divide_error:
   exception_no_errorcode(0)

debug_exception:			   // 1
	exception_no_errorcode(1)

not_mask_interrupt:			// 2
	exception_no_errorcode(2)

bug_point:				      // 3
	exception_no_errorcode(3)

overflow:				      // 4
	exception_no_errorcode(4)

bounds:					      // 5
	exception_no_errorcode(5)

invalid_opcode:				// 6
	exception_no_errorcode(6)

copr_not_available:			// 7
	exception_no_errorcode(7)

double_fault:				   // 8
	exception_with_errorcode(8)

copr_seg_overrun:			   // 9
	exception_no_errorcode(9)

invalid_tss:				   // a
	exception_with_errorcode(0xa)

segment_not_present:		   // b
	exception_with_errorcode(0xb)

stack_excption:				// c
	exception_with_errorcode(0xc)

general_protection:			// d
	exception_with_errorcode(0xd)

page_fault:					   // e
	exception_with_errorcode(0xe)

copr_error:					   // 10
	exception_no_errorcode(0x10)

exception:
	call	exception_handler
	add	$8, %esp
	iret





// ------------------------
// 系统中断调用入口
// ------------------------
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





// ----------------------------
// bss段数据
// 主要存放内核中使用到的变量
//
//
// ----------------------------
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
