#include "database.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

sqlite3 *db = NULL;

// 初始化数据库连接
int init_database() {
    int rc = sqlite3_open(DATABASE_PATH, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "无法打开数据库: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    printf("数据库连接成功\n");
    return 0;
}

// 创建表结构
int create_tables() {
    char *err_msg = NULL;
    
    // 创建花卉基本信息表
    const char *create_plants_table = 
        "CREATE TABLE IF NOT EXISTS plants ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name VARCHAR(50) NOT NULL,"
        "variety VARCHAR(50),"
        "planting_date DATE,"
        "water_frequency INTEGER,"
        "last_water_date DATE,"
        "last_fertilize_date DATE,"
        "status VARCHAR(20) DEFAULT '正常',"
        "notes TEXT"
        ");";
    
    // 创建养护操作记录表
    const char *create_care_records_table = 
        "CREATE TABLE IF NOT EXISTS care_records ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "plant_id INTEGER NOT NULL,"
        "operation_type VARCHAR(20) NOT NULL,"
        "operation_date DATETIME NOT NULL,"
        "details TEXT,"
        "amount VARCHAR(20),"
        "FOREIGN KEY (plant_id) REFERENCES plants(id)"
        ");";
    
    // 创建养护提醒设置表
    const char *create_reminders_table = 
        "CREATE TABLE IF NOT EXISTS reminders ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "plant_id INTEGER NOT NULL,"
        "reminder_type VARCHAR(20) NOT NULL,"
        "frequency INTEGER NOT NULL,"
        "last_reminder_date DATE,"
        "is_active BOOLEAN DEFAULT 1,"
        "FOREIGN KEY (plant_id) REFERENCES plants(id)"
        ");";
    
    // 执行创建表的SQL语句
    int rc = sqlite3_exec(db, create_plants_table, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "创建plants表失败: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }
    
    rc = sqlite3_exec(db, create_care_records_table, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "创建care_records表失败: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }
    
    rc = sqlite3_exec(db, create_reminders_table, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "创建reminders表失败: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }
    
    printf("数据库表创建成功\n");
    return 0;
}

// 关闭数据库连接
void close_database() {
    if (db) {
        sqlite3_close(db);
        db = NULL;
        printf("数据库连接已关闭\n");
    }
}

// 执行SQL语句（无返回结果）
int execute_sql(const char* sql) {
    char *err_msg = NULL;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL执行失败: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }
    return 0;
}

// 检查表是否存在的回调函数
static int table_exists_callback(void* data, int argc, char** argv, char** col_names) {
    *(int*)data = 1;
    return 0;
}

// 检查表是否存在
int table_exists(const char* table_name) {
    char sql[256];
    snprintf(sql, sizeof(sql), 
             "SELECT name FROM sqlite_master WHERE type='table' AND name='%s';", 
             table_name);
    
    char *err_msg = NULL;
    int exists = 0;
    
    int rc = sqlite3_exec(db, sql, table_exists_callback, &exists, &err_msg);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "检查表存在失败: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }
    
    return exists;
}