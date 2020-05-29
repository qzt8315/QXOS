#include "type.h"
#include "const.h"
#include "proto.h"
#include "krllibc.h"
#include "protect.h"
#include "proc.h"
#include "global.h"
#include "hwint.h"
#include "color.h"

// 函数声明
PRIVATE void hwint_handler_clock();

// 硬件中断处理
// 中断处理需要快速完成
PUBLIC void hwint_handler(u32 hwint_no){
	switch (hwint_no)
	{
	// 时间中断处理
	case _NR_GET_TICKS:
		hwint_handler_clock();
		return;
		break;
	
	default:
		break;
	}
}

// 时钟中断处理
void hwint_handler_clock(){
	// 减少中断处理时间
	// display_str_colorful("*", black<<4 | light_gray);
    ticks++;
	n_timeSlice--;
	if(n_timeSlice <=0){
		int	i;
		for(i=1; i<NR_TASKS; i++){
			int index = (iCurProc+i)%NR_TASKS;
			if(proc_table[index].status == s_running){
				b_SwitchProc = 1;
				iCurProc = index;
				cur_proc = &proc_table[index];
			}
		}
	}
}


// 初始化8253可编程定时器
void init_8253(){
	u16 counter0 = (u16)(FREQUENCE_8253/(1000/COLOCK_DUR));
	out_byte(_8253_MODE_PORT, _8253_WRITE_COUNTER0);
	out_byte(_8253_COUNTER0_PORT, (u8)(counter0&0x00ff));
	out_byte(_8253_COUNTER0_PORT, (u8)((counter0>>8)&0x00ff));
}