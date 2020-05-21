#ifndef _CGA_H_
#define _CGA_H_
#include "type.h"
#include "color.h"

// 显存基本操作单位
typedef	struct s_gmchar
{
	// ascii
	u8	ch;
	// 颜色
	u8	color;
} GMCHAR;
#endif