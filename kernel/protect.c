#include "type.h"
#include "const.h"
#include "proto.h"
#include "krllibc.h"
#include "protect.h"
#include "proc.h"
#include "global.h"



// 异常处理
void	divide_error();
void	debug_exception();
void	not_mask_interrupt();
void	bug_point();
void	overflow();
void	bounds();
void	invalid_opcode();
void	copr_not_available();
void	double_fault();
void	copr_seg_overrun();
void	invalid_tss();
void	segment_not_present();
void	stack_excption();
void	general_protection();
void	page_fault();
void	copr_error();

// 硬件中断
void	hwint00();
void	hwint01();
void	hwint02();
void	hwint03();
void	hwint04();
void	hwint05();
void	hwint06();
void	hwint07();
void	hwint08();
void	hwint09();
void	hwint10();
void	hwint11();
void	hwint12();
void	hwint13();
void	hwint14();
void	hwint15();

// 系统调用
void	sys_call();
// 
void	init_8253();


PUBLIC void exception_handler(u32 int_no, u32 error_code, u32 eip, u32 cs, u32 eflags){
	u8 char_color = 0x74;
	char * error_msg[] = {"#DE Divide Error",
			"#DB RESERVED",
			"--  NMI Interrupt",
			"#BP Breakpoint",
			"#OF Overflow",
			"#BR BOUND Range Exceeded",
			"#UD Invalid Opcode (Undefined Opcode)",
			"#NM Device Not Available (No Math Coprocessor)",
			"#DF Double Fault",
			"    Coprocessor Segment Overrun(reserved)",
			"#TS Invalid Tss",
			"#NP Segment Not Present",
			"#SS Stack Segment Fault",
			"#GP General Protection",
			"#PF Page Fault",
			"--  (Intel reserved Do not use)",
			"#MF x86 FPU Floating-Point Error (math fault)",
			"#AC Alignment Check",
			"#MC Machine Check",
			"#XF SIMD Floating-Point Exception",
			};
	
	display_str_colorful("Exception! --> ", char_color);
	display_str_colorful(error_msg[int_no], char_color);
	display_str_colorful("\n\n", char_color);

	char int_str[11];

	display_str_colorful("EFLAGS:", char_color);
	uint2str(eflags, int_str);
	display_str_colorful(int_str, char_color);

	display_str_colorful("  CS:", char_color);
	uint2str(cs, int_str);
	display_str_colorful(int_str, char_color);

	display_str_colorful("  EIP:", char_color);
	uint2str(eip, int_str);
	display_str_colorful(int_str, char_color);

	if(error_code != 0xffffffff){
		display_str_colorful("  Error code:", char_color);
		uint2str(error_code, int_str);
		display_str_colorful(int_str, char_color);
	}
	display_str_colorful("\n", char_color);
}


// 初始化中断描述符
PUBLIC	int init_idt_desc(int int_no, u8 desc_type, int_handler handler, u8 privilege){
	GATE* p_gate = &(idt[int_no]);
	u32   base   = (u32)handler;
	p_gate->offset_low16 = (u16)(base & 0xffff);
	p_gate->selector = Selector_Code;
	p_gate->dcount	= 0;
	p_gate->attr	= desc_type | (privilege<<5);
	p_gate->offset_high16 = (u16)((base >> 16) & 0xffff);
}

PUBLIC	void init_prot(){
	init8259();
	init_8253();
	
	// 异常
	
	init_idt_desc(INT_VECTOR_DIVIDE_ERROR,		DA_386IGate,
			divide_error,			PRIVILEGY_KRL);
	
	init_idt_desc(INT_VECTOR_DEBUG_EXCEPTION,	DA_386IGate,
			debug_exception,		PRIVILEGY_KRL);
	
	init_idt_desc(INT_VECTOR_NO_MASK_INTERRUPT,	DA_386IGate,
			not_mask_interrupt,		PRIVILEGY_KRL);
	
	init_idt_desc(INT_VECTOR_BUG_POINT,		DA_386IGate,
			bug_point,			PRIVILEGY_KRL);
	
	init_idt_desc(INT_VECTOR_OVERFLOW,		DA_386IGate,
			overflow,			PRIVILEGY_KRL);
	
	init_idt_desc(INT_VECTOR_BOUNDS,		DA_386IGate,
			bounds,				PRIVILEGY_KRL);
	
	init_idt_desc(INT_VECTOR_INVALID_OPCODE,	DA_386IGate,
			invalid_opcode,			PRIVILEGY_KRL);
	
	init_idt_desc(INT_VECTOR_COPR_NOT_AVAIL,	DA_386IGate,
			copr_not_available,		PRIVILEGY_KRL);
	
	init_idt_desc(INT_VECTOR_DOUBLE_FAULT,		DA_386IGate,
			double_fault,			PRIVILEGY_KRL);
	
	init_idt_desc(INT_VECTOR_COPR_SEG_OVERRUN,	DA_386IGate,
			copr_seg_overrun,		PRIVILEGY_KRL);
	
	init_idt_desc(INT_VECTOR_INVALID_TSS,		DA_386IGate,
			invalid_tss,			PRIVILEGY_KRL);
	
	init_idt_desc(INT_VECTOR_SEGMENT_NOT_PRESENT,	DA_386IGate,
			segment_not_present,		PRIVILEGY_KRL);
	
	init_idt_desc(INT_VECTOR_STACK_EXCEPTION,	DA_386IGate,
			stack_excption,			PRIVILEGY_KRL);
	
	init_idt_desc(INT_VECTOR_GENERAL_EXCEPTION,	DA_386IGate,
			general_protection,		PRIVILEGY_KRL);
	
	init_idt_desc(INT_VECTOR_PAGE_FAULT,		DA_386IGate,
			page_fault,			PRIVILEGY_KRL);
	
	init_idt_desc(INT_VECTOR_COPR_ERROR,		DA_386IGate,
			copr_error,			PRIVILEGY_KRL);

	// 硬件中断
	
	
	init_idt_desc(INT_VECTOR_IRQ0 + 0,		DA_386IGate,
			hwint00,			PRIVILEGY_KRL);
	init_idt_desc(INT_VECTOR_IRQ0 + 1,		DA_386IGate,
			hwint01,			PRIVILEGY_KRL);
	init_idt_desc(INT_VECTOR_IRQ0 + 2,		DA_386IGate,
			hwint02,			PRIVILEGY_KRL);
	init_idt_desc(INT_VECTOR_IRQ0 + 3,		DA_386IGate,
			hwint03,			PRIVILEGY_KRL);
	init_idt_desc(INT_VECTOR_IRQ0 + 4,		DA_386IGate,
			hwint04,			PRIVILEGY_KRL);
	init_idt_desc(INT_VECTOR_IRQ0 + 5,		DA_386IGate,
			hwint05,			PRIVILEGY_KRL);
	init_idt_desc(INT_VECTOR_IRQ0 + 6,		DA_386IGate,
			hwint06,			PRIVILEGY_KRL);
	init_idt_desc(INT_VECTOR_IRQ0 + 7,		DA_386IGate,
			hwint07,			PRIVILEGY_KRL);
	
	
	init_idt_desc(INT_VECTOR_IRQ8 + 0,		DA_386IGate,
			hwint08,			PRIVILEGY_KRL);
	init_idt_desc(INT_VECTOR_IRQ8 + 1,		DA_386IGate,
			hwint09,			PRIVILEGY_KRL);
	init_idt_desc(INT_VECTOR_IRQ8 + 2,		DA_386IGate,
			hwint10,			PRIVILEGY_KRL);
	init_idt_desc(INT_VECTOR_IRQ8 + 3,		DA_386IGate,
			hwint11,			PRIVILEGY_KRL);
	init_idt_desc(INT_VECTOR_IRQ8 + 4,		DA_386IGate,
			hwint12,			PRIVILEGY_KRL);
	init_idt_desc(INT_VECTOR_IRQ8 + 5,		DA_386IGate,
			hwint13,			PRIVILEGY_KRL);
	init_idt_desc(INT_VECTOR_IRQ8 + 6,		DA_386IGate,
			hwint14,			PRIVILEGY_KRL);
	init_idt_desc(INT_VECTOR_IRQ8 + 7,		DA_386IGate,
			hwint15,			PRIVILEGY_KRL);


	// 开启系统调用
	init_idt_desc(0x80,		DA_386IGate,
			sys_call,			PRIVILEGY_USER);
}


// 初始化描述符
PUBLIC	int init_desc(DESCRIPTOR* p_desc, u32 base, u32 limit, u16 attr, u8 dpl){
	if(p_desc == NULL)
		return	-1;
	p_desc->limit_low16 = (u16)(limit & 0xffff);
	p_desc->base_low16	 = (u16)(base & 0xffff);
	p_desc->base_mid8	 = (u8)((base>>16) & 0xff);
	p_desc->attr1	 	 = (u8)((attr & 0x9f) | ((dpl & 0x3)<<5) );
	p_desc->attr2_limit_high4	 = (u8)( ((attr>>8) & 0xf0) | ((limit>>16)&0x0f)  );
	p_desc->base_high8	 = (u8)( (base>>24) & 0xff );
	return	0;
}

PUBLIC	void	sendEOI2Master(){
	out_byte(INT_M_PORT0, EOI);
}

PUBLIC	void	sendEOI2Slave(){
	out_byte(INT_S_PORT0, EOI);
}

PUBLIC	void	disableIRQ(u8 irq){
	u8 bmask;
	if(irq < 8){
		bmask = in_byte(INT_M_PORT1);
		bmask = bmask | (1 << irq);
		out_byte(INT_M_PORT1, bmask);
	}else{
		bmask = in_byte(INT_S_PORT1);
		bmask = bmask | (1 << (irq-8));
		out_byte(INT_S_PORT1, bmask);
	}
}


PUBLIC	void	enableIRQ(u8 irq){
	u8 bmask;
	if(irq < 8){
		bmask = in_byte(INT_M_PORT1);
		bmask = bmask ^ (1 << irq);
		out_byte(INT_M_PORT1, bmask);
	}else{
		bmask = in_byte(INT_S_PORT1);
		bmask = bmask ^ (1 << (irq-8));
		out_byte(INT_S_PORT1, bmask);
	}
}