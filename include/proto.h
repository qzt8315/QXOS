// 用于存放汇编文件中函数的原型
#ifndef	__PROTO_H__
#define	__PROTO_H__
#include "const.h"

// 端口输入原型
void out_byte(u16 port, u8 b_byte);

// 端口输出原型
u8 in_byte(u16 port);

// 打印彩色字符串
void display_str_colorful(char *str, u8 color);

// 系统调用
// 获取时钟中断计数
// unsigned int	get_ticks();

// 内存拷贝
PUBLIC	int	Memcpy(void* pDst, void* pSrc, int len);
// 内存初始化
PUBLIC	int	Memset(void* s,int ch, int n);
// 加载tss
void    load_tss(u16    tss_sel);
// 加载ldt
void     load_ldt(u16   ldt_sel);
#endif
