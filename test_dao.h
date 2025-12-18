#ifndef TEST_DAO_H
#define TEST_DAO_H

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "database.h"
#include "dao/plants_dao.h"
#include "dao/care_records_dao.h"
#include "dao/reminders_dao.h"

// 测试结果宏
#define TEST_PASSED 0
#define TEST_FAILED 1

#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            printf("✗ 测试失败: %s (文件: %s, 行: %d)\n", message, __FILE__, __LINE__); \
            return TEST_FAILED; \
        } \
    } while(0)

#define TEST_EQUAL(actual, expected, message) \
    do { \
        if ((actual) != (expected)) { \
            printf("✗ 测试失败: %s (期望: %d, 实际: %d)\n", message, (expected), (actual)); \
            return TEST_FAILED; \
        } \
    } while(0)

#define TEST_STRING_EQUAL(actual, expected, message) \
    do { \
        if (strcmp((actual), (expected)) != 0) { \
            printf("✗ 测试失败: %s (期望: %s, 实际: %s)\n", message, (expected), (actual)); \
            return TEST_FAILED; \
        } \
    } while(0)

// 测试函数声明
int test_plants_dao();
int test_care_records_dao();
int test_reminders_dao();

// 辅助函数
void get_current_date(char* date_str);
void get_current_datetime(char* datetime_str);

#endif