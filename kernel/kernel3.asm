; 硬件中断处理
%include "kernel.inc"

; 变量
extern	irq_table
extern  STACKTOP
extern	proc_table

; 函数
extern	hwint_handler
extern  sendEOI2Master
extern  sendEOI2Slave
extern  disableIRQ
extern  enableIRQ
extern  sendEOI2Master
extern  sendEOI2Slave
extern	display_str_colorful
extern	set_TSS_stack_ring0
extern	load_tss

[section .text]
align   32
[bits   32]

global	restart

sss		db 	"B"
		db	0

; 硬件中断
global	hwint00
global	hwint01
global	hwint02
global	hwint03
global	hwint04
global	hwint05
global	hwint06
global	hwint07
global	hwint08
global	hwint09
global	hwint10
global	hwint11
global	hwint12
global	hwint13
global	hwint14
global	hwint15
global	saveregs

saveregs:
	; 保存寄存器的值
	pushad
	push	ds
	push	es
	push	fs
	push	gs

	; 切换到内核栈
	mov		bx, ss
	mov		ds, bx
	mov		es, bx
	mov		ebp, esp

	push	restart
	jmp		[ebp + RETADDR_OFF]

; 切换回进程
restart:
	push	proc_table+_4K+76
	push	0x10
	call	set_TSS_stack_ring0
	add		esp, 8
	; 注意不能重复加载同一个tss,会导致加载失败，同时
	;push	TSS_SELECTOR
	;call	load_tss
	;add		esp, 4
	; mov		esp, proc_table+_4K+76
	;mov	ax,	TSS_SELECTOR
	;ltr	ax
	;lldt	[esp+PROC_LDT_OFF]
	mov		ax, 0x28
	lldt	ax
	pop	gs
	pop	fs
	pop	es
	pop	ds
	popad
	add	esp, 8

	iretd

hwint00:
    irq_master  0
	;iretd
	;push	0
	;jmp	hwint
hwint01:
    irq_master  1
hwint02:
    irq_master  2
hwint03:
    irq_master  3
hwint04:
    irq_master  4
hwint05:
    irq_master  5
hwint06:
    irq_master  6
hwint07:
    irq_master  7
hwint08:
    irq_slave   8
hwint09:
    irq_slave   9
hwint10:
    irq_slave   10
hwint11:
    irq_slave   11
hwint12:
    irq_slave   12
hwint13:
    irq_slave   13
hwint14:
    irq_slave   14
	;push	14
	;jmp	hwint
hwint15:
    irq_slave   15