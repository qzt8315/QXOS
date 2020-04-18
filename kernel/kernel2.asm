; 异常处理
%include "kernel.inc"


extern	exception_handler
extern	proc_table

[section .text]
align   32
[bits   32]

; 异常处理
global	divide_error
global	debug_exception
global	not_mask_interrupt
global	bug_point
global	overflow
global	bounds
global	invalid_opcode
global	copr_not_available
global	double_fault
global	copr_seg_overrun
global	invalid_tss
global	segment_not_present
global	stack_excption
global	general_protection
global	page_fault
global	copr_error

; 异常处理
divide_error:				; 0
	push	0xffffffff
	push	0
	jmp	exception

debug_exception:			; 1
	push	0xffffffff
	push	1
	jmp	exception

not_mask_interrupt:			; 2
	push	0xffffffff
	push	2
	jmp	exception

bug_point:				; 3
	push	0xffffffff
	push	3
	jmp	exception

overflow:				; 4
	push	0xffffffff
	push	4
	jmp	exception

bounds:					; 5
	push	0xffffffff
	push	5
	jmp	exception

invalid_opcode:				; 6
	push	0xffffffff
	push	6
	jmp	exception

copr_not_available:			; 7
	push	0xffffffff
	push	7
	jmp	exception

double_fault:				; 8
	push	8
	jmp	exception

copr_seg_overrun:			; 9
	push	0xffffffff
	push	9
	jmp	exception

invalid_tss:				; a
	push	0xa
	jmp	exception

segment_not_present:		; b
	push	0xb
	jmp	exception

stack_excption:				; c
	push	0xc
	jmp	exception

general_protection:			; d
	push	0xd
	jmp	exception

page_fault:					; e
	push	0xe
	jmp	exception

copr_error:					; 10
	push	0x10
	jmp	exception

exception:
	call	exception_handler
	add	esp, 8
	iret