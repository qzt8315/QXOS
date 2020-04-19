; 编译 nasm -f elf -o kernel.o kernel.asm
; 链接 ld -s -m elf_i386 -Ttext 0x30400 -o QXOSKRL.BIN kernel.o
; 最初内核进入入口，并初始化内核
%include "kernel.inc"

SELECTOR_KERNEL_CS	equ	8

extern	cstart
extern	gdt_ptr
extern	idt_ptr

extern	kernel_main

extern	init_proc
extern	proc_table

; 内核栈
[section .bss]
global	STACKTOP

STACKSPACE:	resb	STACK_SIZE
STACKTOP:

[section .text]
align   32
[bits   32]

global	_start

_start:
	mov	esp, STACKTOP
	sgdt	[gdt_ptr]
	call	cstart
	lgdt	[gdt_ptr]
	lidt	[idt_ptr]

	jmp	SELECTOR_KERNEL_CS:csinit

csinit:
	; 新建进程
	call	init_proc

	;int 	20h

	; 转跳新进程
	mov	ax, 0x20
	ltr	ax
	mov	ax, 0x28
	lldt	ax
	mov	esp, proc_table
	pop	gs
	pop	fs
	pop	es
	pop	ds
	popad
	add	esp, 4
	iretd
	jmp	$