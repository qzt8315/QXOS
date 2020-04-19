#include	"type.h"
#include	"const.h"
#include	"protect.h"
#include	"proc.h"
#include	"global.h"
#include	"proto.h"



PUBLIC	void	cstart(){
	// 将原本的GDT复制到新位置并初始化GDT
	Memcpy((void *)gdt,
		(void*)(*((u32*)(&gdt_ptr[2]))),
		*((u16*)(gdt_ptr)) + 1
		);
	u16 *ptr_limit = (u16*)gdt_ptr;
	u32 *ptr_base  = (u32*)(&gdt_ptr[2]);
	*ptr_limit = GDT_SIZE * sizeof(DESCRIPTOR) - 1;
	*ptr_base  = (u32)gdt;

	// 初始化IDT
	u16 *iptr_limit = (u16*)idt_ptr;
	u32 *iptr_base  = (u32*)(&idt_ptr[2]);
	*iptr_limit = IDT_SIZE * sizeof(GATE) - 1;
	*iptr_base  = (u32)idt;

	// 初始化当前显示地方
	dis_pos = 0;

	// 中断重入控制初始化
	k_reenter = 0;

	// char *strng = "123\n456\n789";
	// display_str_colorful(strng, 0x74);
	init_prot();
}

