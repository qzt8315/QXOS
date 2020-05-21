#ifndef _COLOR_H_
#define _COLOR_H_
// CGA模式下16种颜色及其代码
// 颜色计算方式
// red   := 2/3×(colorNumber & 4)/4 + 1/3×(colorNumber & 8)/8
// green := 2/3×(colorNumber & 2)/2 + 1/3×(colorNumber & 8)/8
// blue  := 2/3×(colorNumber & 1)/1 + 1/3×(colorNumber & 8)/8
// if colorNumber = 6 then green := green / 2
// 参考资料: https://en.wikipedia.org/wiki/Color_Graphics_Adapter
#define black           0x0         // rgb(0    ,0      ,0)     黑色
#define blue            0x1         // rgb(0    ,0      ,170)   蓝色
#define green           0x2         // rgb(0    ,170    ,0)     绿色
#define cyan            0x3         // rgb(0    ,170    ,170)   青色
#define red             0x4         // rgb(170  ,0      ,0)     红色
#define magenta         0x5         // rgb(170  ,0      ,170)   洋红色
#define brown           0x6         // rgb(170  ,55     ,0)     棕色
#define light_gray      0x7         // rgb(170  ,170    ,170)   浅灰色
#define dark_gray       0x8         // rgb(85   ,85     ,85)    深灰色
#define light_blue      0x9         // rgb(85   ,85     ,255)   浅蓝色
#define light_green     0xa         // rgb(85   ,255    ,85)    浅绿色
#define light_cyan      0xb         // rgb(85   ,255    ,255)   浅青色
#define light_red       0xc         // rgb(255  ,85     ,85)    浅红色
#define light_magenta   0xd         // rgb(255  ,85     ,255)   浅洋红色
#define yellow          0xe         // rgb(255  ,255    ,85)    黄色
#define white           0xf         // rgb(255  ,255    ,255)   白色
#endif