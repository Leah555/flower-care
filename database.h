#ifndef DATABASE_H
#define DATABASE_H

#include "sqlite3.h"

#define DATABASE_PATH "./flower_care.db"

// 数据库连接句柄
extern sqlite3 *db;

// 初始化数据库
int init_database();

// 创建表结构
int create_tables();

// 关闭数据库连接
void close_database();

// 执行SQL语句（无返回结果）
int execute_sql(const char* sql);

// 检查表是否存在
int table_exists(const char* table_name);

// 检查所有必需的表是否存在
int check_tables_exist();

// 手动创建数据库（独立函数，可在任何地方调用）
int create_database_manually();

// 重置数据库（删除并重新创建）
int reset_database();

#endif