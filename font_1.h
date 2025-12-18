#ifndef __FONT_1_H__
#define __FONT_1_H__

#include "font.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 函数功能：字体初始化
 * 函数名：fontInit
 * @fontPath：字库文件（.ttf）的路径,如果为NULL则会初始化6818默认的字体库
 * 返回值：
 *      void
 */ 
void fontInit(const char *fontPath);  
/**
 * 函数功能：显示字符串到LCD
 * 函数名：showString
 * @plcd：lcd屏幕的映射
 * @str：需要显示的字符串
 * @size：显示字符串字体的大小（字体的高度）
 * @font_color:字体的颜色（十六进制RGB值）
 * @x，y：字体显示的位置
 * @bg_color：字体背景版的颜色
 * 返回值：
 *      int  返回显示字符串的宽度
 */
int showString(int *plcd, const char *str,int size, int font_color, int x, int y, int bg_color);
/**
 * 函数功能：字库关闭
 * 函数名：fontClose
 * 无参数
 * 返回值：
 *      void
 */
void fontClose(void);


#ifdef __cplusplus
}
#endif

#endif