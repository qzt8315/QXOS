// 线程相关函数
#include "type.h"
#include "const.h"
#include "protect.h"
#include "proc.h"
#include "global.h"
#include "proto.h"

// 初始化多进程
PUBLIC  void    init_multiproc(){
    // 初始化TSS段
	init_desc(&gdt[INDEX_TSS], &proc_tss, sizeof(TSS) - 1, DA_386TSS, PRIVILEGY_TASK);
	TSS* p_tss	= &proc_tss;
    p_tss->io_base = sizeof(TSS);
    // 加载tss
    load_tss(INDEX_TSS * sizeof(DESCRIPTOR));

    // 初始化usergdt
    init_desc(&gdt[INDEX_USER_CODE], 0, 0xfffff, DA_C | DA_32 | DA_LIMIT_4K, DA_DPL3);
    init_desc(&gdt[INDEX_USER_DATA], 0, 0xfffff, DA_DRW | DA_LIMIT_4K, DA_DPL3);

    // 初始化ldt
    Memcpy( ldt, &gdt[INDEX_CODE], sizeof(DESCRIPTOR));
	ldt[0].attr1 = DA_C | DA_DPL1;
	Memcpy(&(ldt[1]), &gdt[INDEX_DATA], sizeof(DESCRIPTOR));
	ldt[1].attr1 = DA_DRW | DA_DPL1;
	init_desc(&gdt[INDEX_LDT1], ldt, LDT_SIZE * sizeof(DESCRIPTOR) - 1, DA_LDT, PRIVILEGY_TASK);
    // 加载ldt
    load_ldt(INDEX_LDT1 * sizeof(DESCRIPTOR));
}

// 设置ring0使用的堆栈
PUBLIC  void    set_TSS_stack_ring0(u32 ss0, u32 esp0){
    TSS* p_tss	= &proc_tss;
    p_tss->ss0  = ss0;
    p_tss->esp0 = esp0;
}

// 设置ring1使用的堆栈
PUBLIC  void    set_TSS_stack_ring1(u32 ss1, u32 esp1){
    TSS* p_tss	= &proc_tss;
    p_tss->ss1  = ss1;
    p_tss->esp1 = esp1;
}


// 设置ring2使用的堆栈
PUBLIC  void    set_TSS_stack_ring2(u32 ss2, u32 esp2){   
    TSS* p_tss	= &proc_tss;
    p_tss->ss2  = ss2;
    p_tss->esp2 = esp2;
}

// 初始化进程表
// p_proctab: 初始化的进程表
// pEnter: 进程入口值
// pStack: 进程堆栈
PUBLIC  PROCESS*    init_proctable(PROCESS* p_proctab, void* pEnter, void* pStack){
    // 填充ldt选择子，进程中不会使用
	p_proctab->ldt_sel      = SELECTOR_LDT_FIRST | SA_RPL_3;
    // 初始化寄存器
    p_proctab->regs.cs      = Selector_User_Code;
    p_proctab->regs.eip     = (u32)pEnter;
    p_proctab->regs.ss      = Selector_User_Data;
    p_proctab->regs.esp     = (u32)pStack;
    p_proctab->regs.ds      = Selector_User_Data;
    p_proctab->regs.es      = Selector_User_Data;
    p_proctab->regs.fs      = Selector_User_Data;
    p_proctab->regs.gs      = Selector_User_Data;
    // IOPL = 1;IF=1(响应可屏蔽中断);
    p_proctab->regs.eflags  = 0x1202;
    // 设置进程状态
    p_proctab->status   = s_running;
}