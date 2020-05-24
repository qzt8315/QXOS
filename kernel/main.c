// 进程测试代码
#include "const.h"
#include "type.h"
#include "protect.h"
#include "proc.h"
#include "global.h"
#include "proto.h"
void			TestA();

extern	u32	USERSTACKTOP_TEMP;

void	TestA(){
	int a;
	while(1){
		a = get_ticks();
		display_str_colorful("A", black<<4 | white);
		delay_ms(10);
	}
}
