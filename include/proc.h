#ifndef	__PROC_H__
#define	__PROC_H__
#include "vm.h"

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
	u32	error_code;
	u32	eip;
	u32	cs;
	u32	eflags;
	u32	esp;
	u32	ss;
} STACK_FRAME;

typedef	struct	s_proc{
	// 进程内核栈， 4K
	u8			proc_stack[PROC_STACKSIZE];
	STACK_FRAME	regs;
	u16			ldt_sel;
	u32			pid;
	u8			procStatus;
	//	进程状态:停止、运行、睡眠、僵尸
	#define		s_stopped		0;
	#define		s_running		1;
	#define		s_sleeping		2;
	#define		s_zombie		3;
	char		p_name[16];
} PROCESS;

PUBLIC  void    init_multiproc();
PUBLIC  void    set_TSS_stack_ring0(u32 ss0, u32 esp0);

#endif
