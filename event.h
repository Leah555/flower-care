#ifndef __EVENT_H__
#define __EVENT_H__

//定义表示一个点坐标的结构体
struct point
{
    int x;
    int y;
};

//定义一个触摸屏信息结构体
#include <linux/input.h> 

/*
**函数功能：获取触摸的坐标
**函数名：get_xy
**参数：无
**返回值：
    struct point 返回获取到的坐标值（横坐标，纵坐标）
*/
struct point get_xy();
#endif