; 硬件中断处理
%include "kernel.inc"

; 变量
extern	irq_table
extern	k_reenter
extern  STACKTOP

; 函数
extern	hwint_handler
extern  sendEOI2Master
extern  sendEOI2Slave
extern  disableIRQ
extern  enableIRQ
extern  sendEOI2Master
extern  sendEOI2Slave

[section .text]
align   32
[bits   32]

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

saveregs:
	; 保存寄存器的值
	pushad
	push	ds
	push	es
	push	fs
	push	gs

	; 切换到内核栈
	mov		ax, ss
	mov		ds, ax
	mov		es, ax
	mov		ebp, esp

	mov		eax, [k_reenter]
	inc		eax
	mov		[k_reenter], eax
	cmp		eax, 0
	jg		.re_irq
	mov		esp, STACKTOP
.re_irq:
	jmp		[ebp + RETADDR_OFF]

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