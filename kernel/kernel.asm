; 编译 nasm -f elf -o kernel.o kernel.asm
; 链接 ld -s -m elf_i386 -Ttext 0x30400 -o QXOSKRL.BIN kernel.o
; 最初内核进入入口，并初始化内核
%include "kernel.inc"

SELECTOR_KERNEL_CS	equ	8

extern	cstart
extern	gdt_ptr
extern	idt_ptr
extern	k_reenter
extern	pPDETable

extern	kernel_main

extern	init_proc
extern	proc_table
extern	restart
extern	init_vm
extern	kernelUnMap

; 内核栈
[section .bss]
global	STACKTOP
global	ARDS_SPACE
global	PAGESPACE
global	_EPAGESPACE
global	MEMFREEBLOCKSPACE
global	_EMEMFREEBLOCKSPACE
global	USERSTACKTOP_TEMP

STACKSPACE:	resb	STACK_SIZE
STACKTOP:
ARDS_SPACE:	resb	ARDS_SIZE*ARDS_COUNT
PAGESPACE:	resb	(VKRLADDRSIZE+VKRLADDRSIZE)/_4K*PAGEITEMSIZE
_EPAGESPACE:
; 注意最多512项
MEMFREEBLOCKSPACE:	resb	_4K
_EMEMFREEBLOCKSPACE:
USERSTACK_TEMP:	resb	_4K
USERSTACKTOP_TEMP:

[section .text]
align   32
[bits   32]

global	_start
global	load_tss

_start:
	mov		esp, V2P(STACKTOP)
	sgdt	[V2P(gdt_ptr)]
	call	init_vm
	mov		eax, [V2P(pPDETable)]
	or		eax, 8
	mov		cr3, eax
	lgdt	[gdt_ptr]
	; 切换堆栈
	mov		esp, STACKTOP
	; mov		eax, te
 	; jmp		eax
	jmp		8:te
te:
	; 取消内核物理地址直接映射
	call	kernelUnMap
	call	cstart
	lidt	[idt_ptr]
	; 新建进程
	call	init_proc

	;int 	20h

	; 转跳新进程
	push	TSS_SELECTOR
	call	load_tss
	add		esp, 4
	mov		ax, 0x28
	lldt	ax
	inc 	dword [k_reenter]
	jmp		restart
	jmp	$

; void	load_tss(u16 gdt_selector)
load_tss:
	ltr	word [esp+4]
	ret

; void	load_ldt(u16 gdt_selector)
load_ldt:
	lldt	word [esp+4]
	ret
