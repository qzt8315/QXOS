#include	"type.h"
#include	"const.h"
#include	"protect.h"
#include	"proc.h"
#include	"global.h"
#include	"proto.h"



PUBLIC	void	cstart(){
	// 初始化IDT
	u16 *iptr_limit = (u16*)idt_ptr;
	u32 *iptr_base  = (u32*)(&idt_ptr[2]);
	*iptr_limit = IDT_SIZE * sizeof(GATE) - 1;
	*iptr_base  = (u32)idt;
	
	// 初始化中断和异常
	init_prot();

	// 加载idt
	__asm__(
		"lidt	(%0)":
		:
		"m"(idt_ptr):
	);

	// 初始化当前显示地方
	dis_pos = 0;

	// 初始化时钟
	ticks = 0;

	// char *strng = "123\n456\n789";
	// display_str_colorful(strng, 0x74);

	//初始化多进程
	init_multiproc();
}

