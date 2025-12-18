#ifndef DAO_COMMON_H
#define DAO_COMMON_H

#include "../database.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// 通用返回码定义
typedef enum {
    DAO_SUCCESS = 0,
    DAO_ERROR = -1,
    DAO_NOT_FOUND = -2,
    DAO_INVALID_PARAM = -3,
    DAO_DB_ERROR = -4
} DAO_RESULT;

// 通用错误处理宏
#define DAO_CHECK_RESULT(rc) do { \
    if ((rc) != DAO_SUCCESS) { \
        fprintf(stderr, "DAO操作失败: %d at %s:%d\n", (rc), __FILE__, __LINE__); \
    } \
} while(0)

// 通用SQL执行错误处理
#define DAO_CHECK_SQL(rc, err_msg) do { \
    if ((rc) != SQLITE_OK) { \
        fprintf(stderr, "SQL执行失败: %s at %s:%d\n", (err_msg), __FILE__, __LINE__); \
        if (err_msg) sqlite3_free(err_msg); \
    } \
} while(0)

// 通用参数检查
#define DAO_CHECK_PARAM(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "参数检查失败 at %s:%d\n", __FILE__, __LINE__); \
        return DAO_INVALID_PARAM; \
    } \
} while(0)

// 字符串安全复制（防止缓冲区溢出）
static inline int dao_strncpy_safe(char* dest, const char* src, size_t dest_size) {
    if (!dest || !src || dest_size == 0) {
        return DAO_INVALID_PARAM;
    }
    
    size_t src_len = strlen(src);
    if (src_len >= dest_size) {
        src_len = dest_size - 1;
    }
    
    memcpy(dest, src, src_len);
    dest[src_len] = '\0';
    return DAO_SUCCESS;
}

// 获取当前时间戳（格式：YYYY-MM-DD HH:MM:SS）
char* dao_get_current_timestamp();

// 释放字符串内存
void dao_free_string(char* str);

#endif