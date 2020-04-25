// 保护模式的相关数据结构
#ifndef	__KERNEL_PROTECT_H__
#define	__KERNEL_PROTECT_H__

// 中断号
#define	INT_VECTOR_IRQ0	0x20
#define	INT_VECTOR_IRQ8	0x28

// 异常中断号
#define	INT_VECTOR_DIVIDE_ERROR		0
#define	INT_VECTOR_DEBUG_EXCEPTION	1
#define	INT_VECTOR_NO_MASK_INTERRUPT	2
#define	INT_VECTOR_BUG_POINT		3
#define	INT_VECTOR_OVERFLOW		4
#define	INT_VECTOR_BOUNDS		5
#define	INT_VECTOR_INVALID_OPCODE	6
#define	INT_VECTOR_COPR_NOT_AVAIL	7
#define	INT_VECTOR_DOUBLE_FAULT		8
#define	INT_VECTOR_COPR_SEG_OVERRUN	9
#define	INT_VECTOR_INVALID_TSS		10
#define	INT_VECTOR_SEGMENT_NOT_PRESENT	11
#define	INT_VECTOR_STACK_EXCEPTION	12
#define	INT_VECTOR_GENERAL_EXCEPTION	13
#define	INT_VECTOR_PAGE_FAULT		14
#define	INT_VECTOR_COPR_ERROR		16

// EOI
#define	EOI	0x20

// 描述符索引
#define	INDEX_DUMMY	0
#define	INDEX_CODE	1
#define	INDEX_DATA	2
#define	INDEX_VIDEO	3
#define	INDEX_TSS	4
#define	INDEX_LDT1	5

// 描述符大小
#define	DESC_SIZE	8

// 选择子
#define	Selector_Dummy	0x0
#define	Selector_Code	0x8
#define	Selector_Data	0x10
#define	Selector_Video	0x18
#define	Selector_Tss	0x20
#define	Selector_Ldt1	0x28


// 临时选择子
#define	Selector_Ldt_Code	0x0
#define	Selector_Ldt_Data	0x8

// 
#define SELECTOR_LDT_FIRST	Selector_Tss
#define	SELECTOR_KRL_CODE	Selector_Code
#define	SELECTOR_KRL_DATA	Selector_Data

// RPL
#define	SA_RPL_0	0
#define	SA_RPL_1	1
#define	SA_RPL_2	2
#define	SA_RPL_3	3

//TIL
#define	SA_TIL		4
#define	SA_TIG		0

// 特权级
#define	PRIVILEGY_KRL	0
#define	PRIVILEGY_TASK	1
#define	PRIVILEGY_USER	3

#define	DA_C		0x98
#define	DA_DRW		0x92

// 门类型
#define	DA_386TSS	0x89
#define	DA_386IGate	0x8e
#define	DA_LDT		0x82
#define	DA_32		0x4000
#define	DA_LIMIT_4K	0x8000

#define	DA_DPL0		0x0
#define	DA_DPL1		0x20
#define	DA_DPL2		0x40
#define	DA_DPL3		0x60

// 描述符结构
typedef struct s_descriptor {
	u16	limit_low16;
	u16	base_low16;
	u8	base_mid8;
	u8	attr1;
	u8	attr2_limit_high4;
	u8	base_high8;
} DESCRIPTOR;

// 门描述符
typedef struct s_gate {
	u16	offset_low16;
	u16	selector;
	u8	dcount;
	u8	attr;
	u16	offset_high16;
} GATE;

// TSS
typedef struct s_tss{
	u32	backlike;
	u32	esp0;
	u32	ss0;
	u32	esp1;
	u32	ss1;
	u32	esp2;
	u32	ss2;
	u32	cr3;
	u32	eip;
	u32	eflags;
	u32	eax;
	u32	ecx;
	u32	edx;
	u32	ebx;
	u32	esp;
	u32	ebp;
	u32	esi;
	u32	edi;
	u32	es;
	u32	cs;
	u32	ss;
	u32	ds;
	u32	fs;
	u32	gs;
	u32	ldt_selector;
	u16	trap;
	u16	io_base;	// io_base的值大于等于TSS段界限，就表示没有IO许可位图
} TSS;

// 指向中断处理函数的指针
typedef	void (*int_handler)();

// 初始化中断描述符
PUBLIC	int init_idt_desc(int int_no, u8 desc_type, int_handler handler, u8 privilege);

// 初始化描述符
PUBLIC	int init_desc(DESCRIPTOR* p_desc, u32 base, u32 limit, u16 attr, u8 dpl);

// 初始化中断处理
PUBLIC	void init_prot();


#endif
