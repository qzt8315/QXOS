// 内核中使用到的相关的功能
#include "type.h"
#include "const.h"
#include "protect.h"
#include "proc.h"
#include "global.h"
#include "proto.h"
#include "krllibc.h"
#include "cga.h"

// 32位无符号整形转字符串(16进制)
PUBLIC	int	uint2str(u32 num, char *str){
	int cnt=0, i;
	char hex[]="0123456789ABCDEF";
	if(str == NULL)
		return -1;
	*str = '0';
	str++;
	*str = 'x';
	str++;
	if(num == 0){
		*str='0';
		cnt++;
	}
	for(i=28; i>=0; i-=4){
		int index = (num >> i) & 0xf;
		*str=hex[index];
		str++;
		cnt++;
	}
	for(;cnt<8; cnt++, str++){
		*str=0;
	}
	return 0;
}

// 毫秒级延迟
PUBLIC	void 	delay_ms(int ms){
	// int i,j,k;
	// for(i=0; i<n; i++){
	// 	for(j=0; j<1000; j++){
	// 		for(k=0; k<1000; k++){ }
	// 	}
	// }
	int	t = get_ticks()+(ms+COLOCK_DUR-1)/COLOCK_DUR;
	while(get_ticks() < t){}
}

// 复制内存
PUBLIC	int	Memcpy(void* pDst, void* pSrc, int len){
	if((pDst == NULL) || (pSrc == NULL) || (len <= 0))
		return -1;
	u8 *p1=pDst, *p2=pSrc;
	int i =0;
	for(; i<len; i++, p1++, p2++){
	*p1 = *p2;
	}
	return 0;
}

// 内存设值
PUBLIC	int	Memset(void* s,int ch, int n){
	int i;
	u8* p_mem = (u8*)s;
	if((s==NULL)||(n<=0))
		return	-1;
	u8 v=(u8)ch;
	for(i=0; i<n; i++, p_mem++){
		*p_mem = v;
	}
	return 0;
}


u32	get_ticks(){
	int		nr_get_ticks = _NR_GET_TICKS;
	int		n_syscall	 = INT_VECTOR_SYS_CALL;
	u32 	ret;
	__asm__ __volatile__(
		"movl %1,%%eax;"
		CALL_INT(0x80)
		"movl %%eax, %0;":
		"=m"(ret)
		:
		"r"(nr_get_ticks)
		:"%eax"
	);
	return ret;
}




extern	GMCHAR	_VCGAMEM;
// void	disp_str_color(char* str, u8 color){
void	display_str_colorful(char* str, u8 color){
	if(str == NULL || dis_pos >= CGA_CHARS_LINE * CGA_LINES){
		return;
	}
	while(*str != 0 && dis_pos < CGA_CHARS_LINE * CGA_LINES){
		// 换行
		if(*str == '\n'){
			dis_pos = dis_pos-dis_pos%CGA_CHARS_LINE+CGA_CHARS_LINE;
			str++;
			continue;
		}
		int	row	=	dis_pos/CGA_CHARS_LINE;
		int col	= 	dis_pos%CGA_CHARS_LINE;
		pVGAMEM[row][col].ch = *str;
		pVGAMEM[row][col].color = color;
		str++;
		dis_pos++;
	}
}