#ifndef	__PROC_H__
#define	__PROC_H__

typedef	struct	s_stack_frame{
	u32	gs;
	u32	fs;
	u32	es;
	u32	ds;
	u32	edi;
	u32	esi;
	u32	ebp;
	u32	kernel_esp;
	u32	ebx;
	u32	edx;
	u32	ecx;
	u32	eax;
	u32	retaddr;
	u32	eip;
	u32	cs;
	u32	eflags;
	u32	esp;
	u32	ss;
} STACK_FRAME;

typedef	struct	s_proc{
	STACK_FRAME	regs;
	u16			ldt_sel;
	DESCRIPTOR	ldts[LDT_SIZE];
	u32			pid;
	char		p_name[16];
} PROCESS;

PUBLIC  void    init_multiproc();
PUBLIC  void    set_TSS_stack_ring0(u32 ss0, u32 esp0);

#endif
