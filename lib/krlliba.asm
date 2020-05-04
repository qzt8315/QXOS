; 内核中不得不用汇编实现的功能

extern	dis_pos
extern	pVGAMEM

%define	PER_CHAR_LEN	2
%define	LINE_CHARS	80
%define	LINES		25

[section .text]
; 引出各种函数
global	out_byte
global	in_byte
global	display_str_colorful


; 向端口中写入字节
;  void out_byte(u16 port, u8 b_byte)
out_byte:
	push	edx
	push	eax
	mov	edx, [esp + 12]
	mov	al, [esp + 16]
	out	dx, al
	nop
	nop
	pop	eax
	pop	edx
	ret

; 从端口中读取字节
;  u8 in_byte(u16 port)
in_byte:
	push	edx
	mov	edx, [esp + 8]
	in	al, dx
	nop
	nop
	pop	edx
	ret


; 显示彩色文字, 只能运行在内核空间
; void display_str_colorful(char *str, u8 color)
display_str_colorful:
	push	ebp
	mov	ebp, esp
	push	edi
	push	esi
	push	eax
	push	ebx
	push	edx
	push	ecx
	
	; 显存位置
	mov	edi, [dis_pos]
	; 字符串位置
	mov	esi, [ebp + 8]
	cmp	esi, 0
	jz	.show_color_str_end
	; 字体颜色
	mov	ah, [ebp + 12]
.loop_show_color_str:
	cmp	edi, PER_CHAR_LEN * LINE_CHARS * (LINES - 1)
	ja	.show_color_str_end
	lodsb
	cmp	al, 0x0a
	jz	.new_line
	cmp	al, 0
	jz	.show_color_str_end
	add	edi, dword [pVGAMEM]
	mov	[edi], ax
	sub edi, dword [pVGAMEM]
	add	edi, PER_CHAR_LEN
	jmp	.show_next_char
.new_line:
	push	eax
	mov	eax, edi
	mov	bx, PER_CHAR_LEN * LINE_CHARS
	div	bl
	add	al, 1
	mov	bx, PER_CHAR_LEN * LINE_CHARS
	mul	bl
	movzx	edi, ax
	pop	eax
	
.show_next_char:
	jmp	.loop_show_color_str

.show_color_str_end:
	mov	dword [dis_pos], edi
	pop ecx
	pop	edx
	pop	ebx
	pop	eax
	pop	esi
	pop	edi
	pop	ebp
	ret

