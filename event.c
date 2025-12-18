#include"event.h"
#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#ifdef __linux__
#include <linux/input.h>

struct point get_xy()
{
    // 1. 定义保存坐标的变量，初始化为 (-1,-1)
    struct point p = {-1, -1};

    // 2. 打开触摸设备文件 "/dev/input/event0"
    int fd = open("/dev/input/event0", O_RDONLY);
    if (fd < 0) {
        perror("Failed to open touch device /dev/input/event0");
        return p;
    }

    // 3. 定义结构体变量 ev，用于接收输入事件
    struct input_event ev;

    // 4. 循环读取，等待触摸信息
    int re = -1;
    while ((re = read(fd, &ev, sizeof(ev)))) {
        // 判断是否完整读取了一个 input_event 结构
        if (re == sizeof(struct input_event)) {
            // 解析结构体内容
            if (ev.type == EV_ABS) {  // 是绝对坐标事件（触摸移动）
                if (ev.code == ABS_X) {
                    p.x = ev.value;   // 获取横坐标
                } else if (ev.code == ABS_Y) {
                    p.y = ev.value;   // 获取纵坐标
                }
            }
            // 判断触摸是否结束：BTN_TOUCH 松开
            else if (ev.type == EV_KEY && ev.code == BTN_TOUCH && ev.value == 0) {
                break;  // 触摸结束，跳出循环
            }
        }
    }

    // 5. 关闭文件
    close(fd);

    // 6. 打印结果并返回
    printf("x = %d, y = %d\n", p.x, p.y);
    return p;
}
#else
// Windows平台模拟触摸输入
struct point get_xy()
{
    // 在Windows平台上，我们模拟一个固定的触摸点用于测试
    struct point p = {-1, -1};
    
    // 这里可以添加Windows特定的输入处理代码
    // 比如使用Windows API处理鼠标点击作为触摸输入
    // 现在我们简单地返回一个预设值用于测试
    
    printf("Simulated touch input on Windows\n");
    // 模拟点击主菜单的第一个按钮
    p.x = 100;
    p.y = 100;
    
    printf("x = %d, y = %d\n", p.x, p.y);
    return p;
}
#endif