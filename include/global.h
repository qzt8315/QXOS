// 全局变量
#ifdef	GLOBAL_VARIABLES_HERE
#undef	EXTERN
#define	EXTERN
#endif

#include "type.h"
#include "const.h"
#include "mem.h"
#include "protect.h"
#include "proc.h"

// gdt
EXTERN	u8		gdt_ptr[6];
EXTERN	DESCRIPTOR	gdt[GDT_SIZE];

// idt
EXTERN	u8		idt_ptr[6];
EXTERN	GATE		idt[IDT_SIZE];

// 当前显示的行数
EXTERN	u32		dis_pos;

//
EXTERN	PROCESS	proc_table[NR_TASKS];

//
EXTERN	TSS	proc_tss;

//irq中断向量
EXTERN int_handler irq_table[IRQ_NUM];

// 中断重入控制
EXTERN int  k_reenter;

// 时钟计数
EXTERN u32  ticks;

// ARDS
EXTERN ARDS*    p_ards;

// ARDS数量
EXTERN u8       n_ards;

// 空闲内存起始指针
EXTERN MEMFREEBLOCK*    MemFreeStartBlock;

// 内存总空间
u32     MemTotal;

// 内存空余总空间
u32     MemFree;