// 初始化8259a中断编程器
#include "type.h"
#include "const.h"
#include "proto.h"
#include "protect.h"

// 初始化8259中断控制器
void	init8259(){
	// ICW1
	out_byte(INT_M_PORT0, 0x11);
	out_byte(INT_S_PORT0, 0x11);

	// ICW2
	out_byte(INT_M_PORT1, INT_VECTOR_IRQ0);
	out_byte(INT_S_PORT1, INT_VECTOR_IRQ8);

	// ICW3
	out_byte(INT_M_PORT1, 0x04);
	out_byte(INT_S_PORT1, 0x02);

	// ICW4
	out_byte(INT_M_PORT1, 0x01);
	out_byte(INT_S_PORT1, 0x01);

	// OCW1
	out_byte(INT_M_PORT1, 0xff);
	out_byte(INT_S_PORT1, 0xff);
}
