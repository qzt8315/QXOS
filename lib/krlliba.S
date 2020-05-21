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
// .global     display_str_colorful

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