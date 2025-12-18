#include "font_1.h"
#include <stdio.h>
#include <string.h>

// macOS stub 实现，用于编译测试
// 在实际的 Linux/ARM 环境中应使用 libfont_1.so

static int font_initialized = 0;

void fontInit(const char *fontPath) {
    if (fontPath) {
        printf("字体初始化: %s (stub实现)\n", fontPath);
    } else {
        printf("字体初始化: 使用默认字体库 (stub实现)\n");
    }
    font_initialized = 1;
}

int showString(int *plcd, const char *str, int size, int font_color, int x, int y, int bg_color) {
    if (!font_initialized) {
        printf("警告: 字体库未初始化\n");
        return 0;
    }
    
    // stub 实现：只打印到控制台
    printf("[LCD显示] 位置(%d,%d) 大小%d 颜色0x%06X 背景0x%06X: %s\n", 
           x, y, size, font_color, bg_color, str);
    
    // 返回估算的字符串宽度（简单估算：每个字符宽度约为高度的0.6倍）
    return strlen(str) * size * 0.6;
}

void fontClose(void) {
    if (font_initialized) {
        printf("字体库关闭 (stub实现)\n");
        font_initialized = 0;
    }
}

