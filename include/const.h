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

// 系统最大进程数
#define	NR_TASKS	128

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

//系统栈大小 
#define STACK_SIZE  1024

// ARDS大小
#define ARDS_SIZE   20
// ARDS数量
#define ARDS_COUNT  20

// 内核虚拟地址空间
// VKRLADDRSIZE    equ     0x40000000
#define VKRLADDRSIZE    (1<<30)


// 页条目大小
#define PAGEITEMSIZE    4

// 内核实际加载基址 1M
#define K_P_BASE_ADDR	0x100000
// 内核空间虚拟地址基址 3G
#define K_V_BASE_ADDR	0xc0000000

// CGA彩色字符显示
#define CGA_LINES       25
#define CGA_CHARS_LINE  80

#endif
