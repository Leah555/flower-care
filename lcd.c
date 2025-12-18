#include"lcd.h"
#include<stdio.h>
#include<string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#if defined(__linux__) || defined(__APPLE__) || defined(__DARWIN__)
#include <sys/mman.h>
#endif
#include<stdlib.h>


/*
**函数的功能：初始化LCD屏
**函数名：LCD_init
**参数：无
**返回值：LCD类型的变量
*/
LCD LCD_init()
{
    LCD f={-1,NULL};
    f.fd=open("/dev/fb0",O_RDWR);//打开文件
    if(f.fd==-1)
    {
        printf("Failed to open file!\n");
        return f;
    }

    f.plcd = mmap(NULL,800*480*4,PROT_READ|PROT_WRITE,MAP_SHARED,f.fd,0);
    if(f.plcd == NULL)
    {
        printf("mmap error\n");
        close(f.fd); //关闭文件
        return f;
    }

    return f;
}

/*
**函数的功能：给一个点设置参数
**函数名：LCD_show_point
**参数：
    int x,int y ====这个点的坐标位置
    int color   ====颜色值
    int *plcd   ====屏幕第一个像素地址
**返回值：void
*/
void LCD_show_point(int x,int y,int color,int *plcd)
{
    if (plcd == NULL) {
        return;  // 避免空指针访问
    }
    if (x >= 0 && x < 800 && y >= 0 && y < 480) {
        *(plcd+x+y*800) = color;
    }
}

/*
**函数功能：释放空间，关闭屏幕文件
**函数名：LCD_close
**参数：
    int fd      ====文件标识
    int *plcd   ====屏幕第一个像素地址
**返回值：void
*/
void LCD_close(int fd,int *plcd)
{
    munmap(plcd,800*480*4);
    close(fd);
}

/*
**函数功能：根据自己设置的位置画矩形
**函数名：LCD_show_rec
**参数：
    int x,int y ====显示矩形的位置
    int w,int h ====矩形的宽度高度
    int color,int *plcd====矩形的颜色 屏幕的映射
**返回值：
    void
*/
void LCD_show_rec(int x,int y,int w,int h,int color,int *plcd)
{
    for(int j=y;j<y+h;j++)
    {
        for(int i=x;i<x+w;i++)
        {
            LCD_show_point(i,j,color,plcd);
        }
    }
}

/*
**函数的功能：画圆形
**函数名：LCD_show_cir
**参数：
    int x,int y ====圆心的坐标
    int r ====圆的半径
    int color ====圆的颜色
    int *plcd ====屏幕的映射
**返回值：
    void
*/
void LCD_show_cir(int x,int y,int r,int color,int *plcd)
{
    for(int j=0;j<480;j++)
    {
        for(int i=0;i<800;i++)
        {
            if((i-x)*(i-x)+(j-y)*(j-y)<=r*r)
            {
                LCD_show_point(i,j,color,plcd);
            }
        }
    }
}

/**
* 函数功能：在想要的位置显示一张BMP图片
* 函数名：LCD_show_bmp
* 参数：
* char *pathnem 图片路径
* int x,int y 图片显示的位置
* int *plcd 映射
* 返回值：
* void
*/
void LCD_show_bmp(char *pathname, int x, int y, int *plcd)
{
    int fd = open(pathname, O_RDONLY); // 只读打开
    if (fd == -1)
    {
        printf("open %s error\n", pathname);
        return;
    }

    // 判断是否为 BMP 文件
    char cb, cm;
    read(fd, &cb, 1);
    read(fd, &cm, 1);
    if (cb != 'B' || cm != 'M') // 注意：应为 || 而不是 &&
    {
        printf("这不是一张BMP图片文件\n");
        close(fd);
        return;
    }

    // 读取宽度
    lseek(fd, 18, SEEK_SET);
    int w;
    read(fd, &w, 4);

    // 读取高度
    int h;
    read(fd, &h, 4);

    // 读取色深
    lseek(fd, 28, SEEK_SET);
    short d;
    read(fd, &d, 2);

    // 检查是否为 24 位真彩色 BMP（常见情况）
    if (d != 24)
    {
        printf("仅支持24位BMP图片\n");
        close(fd);
        return;
    }

    // 计算每行理论字节数和实际字节数（含填充）
    int line = w * (d / 8); // 理论每行字节数
    int laizi = (4 - (line % 4)) % 4; // 填充字节数（%4 防止 line%4==0 时 laizi=4）
    int line_s = line + laizi; // 实际每行字节数
    int all_size = line_s * h; // 像素数据总大小

    // 分配缓冲区（注意：若 all_size 很大，建议用 malloc）
    char *buf = (char *)malloc(all_size);
    if (!buf)
    {
        printf("内存分配失败\n");
        close(fd);
        return;
    }

    // 定位到像素数据起始位置（BMP 文件头54字节）
    lseek(fd, 54, SEEK_SET);
    read(fd, buf, all_size);

    close(fd);

    // 显示图片
    int i = 0;
    for (int cy = 0; cy < h; cy++)
    {
        for (int cx = 0; cx < w; cx++)
        {
            unsigned char b = buf[i++];
            unsigned char g = buf[i++];
            unsigned char r = buf[i++];
            int color = (r << 16) | (g << 8) | b;
            // 映射坐标：开发板左上角为原点，BMP 图像数据从左下角开始
            LCD_show_point(x + cx, y + h - 1 - cy, color, plcd);
        }
        i += laizi; // 跳过填充字节
    }

    free(buf);
}