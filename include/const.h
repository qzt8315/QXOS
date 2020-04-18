// 保存内核中使用到的常量
#ifndef	__KERNEL_CONST_H__
#define	__KERNEL_CONST_H__

#define	PUBLIC	// PUBLIC 声明该函数可以被外部使用
#define	PRIVATE	static	// PRIVATE 声明函数只能在文件内使用

#define	GDT_SIZE	128	// GDT数量
#define	LDT_SIZE	2	// LDT数量
#define	IDT_SIZE	256	// IDT数量

// 初始化8259需要的参数
#define	INT_M_PORT0	0x20
#define	INT_M_PORT1	0x21
#define	INT_S_PORT0	0xa0
#define	INT_S_PORT1	0xa1

#define	EXTERN	extern

#define	NULL	0

#define	NR_TASKS	1

#define IRQ_NUM     16

#endif
