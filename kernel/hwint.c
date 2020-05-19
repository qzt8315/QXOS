#include "type.h"
#include "const.h"
#include "proto.h"
#include "krllibc.h"
#include "protect.h"
#include "proc.h"
#include "global.h"
#include "hwint.h"

PRIVATE void hwint_handler_clock();

PUBLIC void hwint_handler(u32 hwint_no){
	u8 char_color = 0x74;

	char hwint_str[11];

	// display_str_colorful("hwint no:", char_color);
	// uint2str(hwint_no, hwint_str);
	// display_str_colorful(hwint_str, char_color);
	// display_str_colorful("\n", char_color);
	switch (hwint_no)
	{
	case 0:
		hwint_handler_clock();
		break;
	
	default:
		break;
	}
}

void hwint_handler_clock(){
	display_str_colorful("*", 0x0f);
    ticks++;
}

void init_8253(){
	u16 counter0 = (u16)(FREQUENCE_8253/(1000/COLOCK_DUR));
	out_byte(_8253_MODE_PORT, _8253_WRITE_COUNTER0);
	out_byte(_8253_COUNTER0_PORT, (u8)(counter0&&0x00ff));
	out_byte(_8253_COUNTER0_PORT, (u8)((counter0>>8)&&0x00ff));
}