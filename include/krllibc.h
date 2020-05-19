#ifndef	__KRLLIBC_H__
#define	__KRLLIBC_H__
// -----宏

// 用于汇编中发生中断
#define CALL_INT(nint)    "int $"#nint";"


// -----函数声明

// 整形转字符串(16进制)
PUBLIC	int	uint2str(u32 num, char *str);


// 内存拷贝
PUBLIC	int	Memcpy(void* pDst, void* pSrc, int len);

// 内存设值
PUBLIC	int	Memset(void* s,int ch, int n);

// 毫秒级延迟
void 	delay_ms(int ms);

u32	get_ticks();

#endif
