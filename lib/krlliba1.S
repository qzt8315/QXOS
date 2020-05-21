// 内核中需要汇编实现的函数
.extern     dis_pos
.extern     pVGAMEM

#define	PER_CHAR_LEN	2
#define	LINE_CHARS	    80
#define	LINES		    25

.section    .text
.code32
.align  4

.global     out_byte
.global     in_byte
.global     display_str_colorful

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


// 显示彩色文字
// void display_str_colorful(char *str, u8 color)
display_str_colorful:
    pushl   %ebp
    movl    %esp, %ebp
    pushl   %edi
    pushl   %esi
    pushl   %eax
    pushl   %ebx
    pushl   %edx
    pushl   %ecx

    // 显存位置
    movl    (dis_pos), %edi
    // 字符串位置
    movl    8(%ebp), %esi
    cmp     $0, %esi
    jz      .show_color_str_end
    // 字体颜色
    movb    12(%ebp), %ah
.loop_show_color_str:
    cmp     $(PER_CHAR_LEN * LINE_CHARS * LINES), %edi
    jl      .show_color_str_end
    lodsb
    cmp     $0x0a, %al
    jz      .new_line
    cmp     $0, %al
    jz      .show_color_str_end
    addl    (pVGAMEM), %edi
    movw    %ax, (%edi)
    subl    (pVGAMEM), %edi
    addl    PER_CHAR_LEN, %edi
    jmp     .show_next_char
.new_line:
    pushl   %eax
    movl    %edi, %eax
    movw    $(PER_CHAR_LEN * LINE_CHARS), %bx
    divb    %bl
    addb    $1, %al
    movw    $(PER_CHAR_LEN * LINE_CHARS), %bx
    mulb    %bl
    movzx   %ax, %edi
    popl    %eax

.show_next_char:
    jmp     .loop_show_color_str

.show_color_str_end:
    movl    %edi, (dis_pos)
    popl    %ecx
    popl    %edx
    popl    %ebx
    popl    %eax
    popl    %esi
    popl    %edi
    popl    %ebp
    ret