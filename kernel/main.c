// 进程测试代码
#include "const.h"
#include "type.h"
#include "protect.h"
#include "proc.h"
#include "global.h"
#include "proto.h"
void 			display_str_colorful(char *str, u8 color);
void			TestA();

// 为新线程创建做准备
PUBLIC	void	init_proc(){
	// 初始化TSS段
	// init_desc(&gdt[INDEX_TSS], &proc_tss, sizeof(TSS) - 1, DA_386TSS, PRIVILEGY_TASK);
	
	// 初始化TSS
	//TSS* p_tss	= &proc_tss;
	set_TSS_stack_ring0(Selector_Data, &(proc_table->ldt_sel));
	//p_tss->ss0	= Selector_Data;
	//p_tss->esp0	= &(proc_table->ldt_sel);
	//p_tss->io_base	= sizeof(TSS);
	
	// 初始化进程表
	PROCESS* process = proc_table;
	process->ldt_sel = SELECTOR_LDT_FIRST | SA_RPL_1;
	Memcpy( process->ldts, &gdt[INDEX_CODE], sizeof(DESCRIPTOR));
	process->ldts[0].attr1 = DA_C | DA_DPL1;
	Memcpy(&(process->ldts[1]), &gdt[INDEX_DATA], sizeof(DESCRIPTOR));
	process->ldts[1].attr1 = DA_DRW | DA_DPL1;

	init_desc(&gdt[INDEX_LDT1], process->ldts, LDT_SIZE * sizeof(DESCRIPTOR) - 1, DA_LDT, PRIVILEGY_TASK);

	process->regs.cs = Selector_Ldt_Code | SA_RPL_1 | SA_TIL;
	process->regs.ds = Selector_Ldt_Data | SA_RPL_1 | SA_TIL;
	process->regs.ss = Selector_Ldt_Data | SA_RPL_1 | SA_TIL;
	process->regs.es = Selector_Ldt_Data | SA_RPL_1 | SA_TIL;
	process->regs.gs = Selector_Video | SA_RPL_1;
	process->regs.eip = (u32)TestA;
	process->regs.esp = 0x400000;
	process->regs.eflags = 0x1202;
}

void	TestA(){
	int a;
	while(1){
		a = get_ticks();
		display_str_colorful("A", 0x0f);
		delay_ms(10);
	}
}
