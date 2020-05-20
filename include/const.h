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

// 8253时钟频率
#define FREQUENCE_8253  1193180L

// 时钟中断间隔时间，单位毫秒
#define COLOCK_DUR  10

// CGA显存物理位置
#define PCGAMEM     0xb8000
#define EPCGAMEM    0xc0000

// 4K
#define N_4K                0x1000


#define PROC_STACKSIZE N_4K

// 获取时钟值的系统调用号
#define	_NR_GET_TICKS 		0
// 系统调用中断号
#define INT_VECTOR_SYS_CALL	0x80

#endif
