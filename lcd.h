#ifndef __LCD_H__
#define __LCD_H__


typedef struct lcd//屏幕信息结构体
{
    int fd;
    int *plcd;
}LCD;

LCD LCD_init();
void LCD_show_point(int x,int y,int color,int *plcd);
void LCD_close(int fd,int *plcd);
void LCD_show_rec(int x,int y,int w,int h,int color,int *plcd);
void LCD_show_cir(int x,int y,int r,int color,int *plcd);
void LCD_show_bmp(char *pathname, int x, int y, int *plcd);
#endif