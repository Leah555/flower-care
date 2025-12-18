#include "dao_common.h"
#include <time.h>

// 获取当前时间戳
char* dao_get_current_timestamp() {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    
    char *timestamp = malloc(20); // YYYY-MM-DD HH:MM:SS + null
    if (!timestamp) {
        return NULL;
    }
    
    strftime(timestamp, 20, "%Y-%m-%d %H:%M:%S", tm_info);
    return timestamp;
}

// 释放字符串内存
void dao_free_string(char* str) {
    if (str) {
        free(str);
    }
}