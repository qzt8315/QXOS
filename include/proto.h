// 用于存放汇编文件中函数的原型
#ifndef	__PROTO_H__
#define	__PROTO_H__
// 端口输入原型
void out_byte(u16 port, u8 b_byte);

// 端口输出原型
u8 in_byte(u16 port);

// 打印彩色字符串
void display_str_colorful(char *str, u8 color);
#endif
