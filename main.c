#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "event.h"
#include "lcd.h"
#include "font_1.h"
#include "font.h"
#include <string.h>
#include <stdlib.h>
#include "sqlite3.h"
#include "database.h"
#include "interface.h"

#define DATABASE_PATH "./flower_care.db"

int main()
{
    printf("正在初始化花卉养护记录系统...\n");
    
    // 初始化数据库
    if (!init_database()) {
        printf("数据库初始化失败\n");
        return -1;
    }
    printf("数据库初始化成功\n");
    
    // 初始化界面
    Interface *interface = interface_init();
    if (!interface) {
        printf("界面初始化失败\n");
        return -1;
    }
    printf("界面初始化成功\n");
    
    // 初始化字库
    fontInit("/usr/share/fonts/DroidSansFallback.ttf");
    
    // 显示主界面
    interface_navigate_to(interface, MAIN_MENU);
    
    printf("系统启动完成，等待用户操作...\n");
    
    // 主事件循环
    while (1) {
        struct point touch_point = get_xy();
        
        if (touch_point.x != -1 && touch_point.y != -1) {
            printf("触摸点坐标: (%d, %d)\n", touch_point.x, touch_point.y);
            interface_handle_touch(interface, touch_point);
        }
        
        usleep(100000); // 100ms延迟，防止过于频繁的触摸检测
    }
    
    // 清理资源
    fontClose();
    interface_cleanup(interface);
    
    printf("系统正常退出\n");
    return 0;
}