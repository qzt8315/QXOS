// 进程测试代码
#include "const.h"
#include "type.h"
#include "protect.h"
#include "proc.h"
#include "global.h"
#include "proto.h"
void			TestA();

extern	u32	USERSTACKTOP_TEMP;

// 为新线程创建做准备
PUBLIC	void	init_proc(){
	// 初始化TSS段
	// init_desc(&gdt[INDEX_TSS], &proc_tss, sizeof(TSS) - 1, DA_386TSS, PRIVILEGY_TASK);
	
	// 初始化TSS
	//TSS* p_tss	= &proc_tss;
	// set_TSS_stack_ring0(Selector_Data, (u32)&(proc_table->ldt_sel));
	//p_tss->ss0	= Selector_Data;
	//p_tss->esp0	= &(proc_table->ldt_sel);
	//p_tss->io_base	= sizeof(TSS);
	
	// 初始化进程表
	PROCESS* process = proc_table;
	process->ldt_sel = SELECTOR_LDT_FIRST | SA_RPL_2;
	process->regs.cs = Selector_User_Code;
	process->regs.ds = Selector_User_Data;
	process->regs.ss = Selector_User_Data;
	process->regs.es = Selector_User_Data;
	process->regs.fs = Selector_User_Data;
	process->regs.gs = Selector_User_Data;
	process->regs.eip = (u32)TestA;
	process->regs.esp = (u32)&USERSTACKTOP_TEMP;
	process->regs.eflags = 0x1202;
}

void	TestA(){
	int a;
	while(1){
		a = get_ticks();
		display_str_colorful("A", black<<4 | white);
		delay_ms(10);
	}
}
