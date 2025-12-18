#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>           // open()
#include <sys/mman.h>        // mmap()
#include "event.h"
#include "lcd.h"
#include "font_1.h"
#include "font.h"
#include <string.h>
#include<stdlib.h>
#include "sqlite3.h"

#define DATABASE_PATH "./sql.db"

#define DEEP_BROWN   0x784315   // 深棕色
#define LIGHT_BEIGE  0xF09B59   // 浅棕色
#define GREEN_BAR    0x22B14C   // 标题绿
#define BLACK        0x000000
#define WHITE        0xFFFFFF

/******************************************
 * 功能：回调函数  作用：打印查询信息
 * 返回值：
 *      int：成功返回0，失败返回其他值
 * 参数：
 *      parg：主调函数传递过来的数据的地址
 *      num：查询到的结果的列数
 *      values：查询到的这条记录每一列的值
 *      titles：每一列结果的标题(列名)
******************************************/
int callBackSelectStuInfo(void* parg,int num,char* values[],char* titles[])
{
    int* pcount = (int*)parg;
    if(*pcount == 0)
    {
        //查询到的第一个联系人的信息，先打印表头
        for (int i = 0; i < num; i++)
        {
            printf("%s\t",titles[i]);
        }
        printf("\n");
    }
    //打印联系人信息
    for (int i = 0; i < num; i++)
    {
        //打印每一列的值
        printf("%s\t",values[i]);
    }
    printf("\n");
    (*pcount)++;
    return 0;
}


int main()
{

    LCD f=LCD_init();
    struct point p = {-1, -1};  // 存储触摸坐标
    //初始化字库
    fontInit("/usr/share/fonts/DroidSansFallback.ttf"); //fontInit(NULL);

    // 检查LCD初始化是否成功
    if (f.fd == -1 || f.plcd == NULL) {
        printf("LCD initialization failed\n");
        return -1;
    }

    //打开或者是创建一个数据库文件  数据库文件存在则打开，不存在则创建
    sqlite3 *ppdb; //用于保存打开数据库的一个链接  === 文件描述符
    int s =  sqlite3_open(DATABASE_PATH,&ppdb);
    if(s != SQLITE_OK)  //如果返回值不等于SQLITE_OK，那么打开数据库失败
    {
        perror("打开数据库失败");
        return -1;
    }

    // while (1) {
    //     // 获取一次触摸坐标
    //     p = get_xy();
    //     // 判断点击区域，并改变屏幕颜色
    //     if (p.x >= 0 && p.x <= 200 && p.y >= 0 && p.y <= 200) {
    //         // 区域1：左上角 [0~100, 0~100] → 白色
    //         for (int y = 0; y < 480; y++) {
    //             for (int x = 0; x < 800; x++) {
    //                 LCD_show_point(x, y, 0xFFFFFF, f.plcd);  // 白色
    //             }
    //         }
    //         LCD_show_rec(100,100,50,50,0xff0000,f.plcd);
    //         LCD_show_rec(100,200,50,50,0xff0000,f.plcd);
    //         LCD_show_cir(300,100,50,0x00ff00,f.plcd);
    //         LCD_show_cir(300,200,50,0x00ff00,f.plcd);
    //         LCD_show_cir(300,300,50,0x00ff00,f.plcd);
    //     }
    //     else if (p.x >= 824 && p.x <= 1023 && p.y >= 400 && p.y <= 599) {
    //         // 区域2：右下角 [924~1023, 500~599] → 黑色
    //         for (int y = 0; y < 480; y++) {
    //             for (int x = 0; x < 800; x++) {
    //                 LCD_show_point(x, y, 0x000000, f.plcd);  // 黑色
    //             }
    //         }
    //     }
    //     else if (p.x >= 0 && p.x <= 200 && p.y >= 400 && p.y <= 599) {
    //         // 区域3：左下角 [0~100, 500~599] → 粉色
    //         for (int y = 0; y < 480; y++) {
    //             for (int x = 0; x < 800; x++) {
    //                 LCD_show_point(x, y, 0xFF00FF, f.plcd);  // 粉色
    //             }
    //         }
    //         LCD_show_bmp("./fkm.bmp",100,0,f.plcd);
    //         LCD_show_bmp("./wx.bmp",500,0,f.plcd);
    //         //定义需要显示的字符串
    //         char *str = "扫我！";
    //         //显示函数
    //         showString(f.plcd,str,50,0xff0000,150,400,0x000000);
    //         //定义需要显示的字符串
    //         char *str2 = "加我！";
    //         //显示函数
    //         showString(f.plcd,str2,50,0xff0000,550,400,0x000000);
    //     }
    //     else if (p.x >= 824 && p.x <= 1023 && p.y >= 0 && p.y <= 200) {
    //         // 区域4：右上角 [924~1023, 0~100] → 显示太极图
    //         // 在屏幕中央绘制一个太极图，半径为160
    //         draw_yin_yang(400, 240, 160, f.plcd);
    //     }
    //     else {
    //         printf("Click outside specified area\n");
    //     }
    //     sleep(1); // 防止重复触发太快
    // }
    
    //关闭字库
    fontClose();
    // 最后释放资源
    LCD_close(f.fd, f.plcd);
    return 0;
}