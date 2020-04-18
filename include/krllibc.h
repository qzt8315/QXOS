#ifndef	__KRLLIBC_H__
#define	__KRLLIBC_H__
// 整形转字符串(16进制)
PUBLIC	int	uint2str(u32 num, char *str);


// 内存拷贝
PUBLIC	int	Memcpy(void* pDst, void* pSrc, int len);

// 内存设值
PUBLIC	int	Memset(void* s,int ch, int n);

#endif
