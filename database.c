#include "database.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <windows.h>
#endif

sqlite3 *db = NULL;

// 检查数据库文件是否存在
static int database_file_exists(const char *filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

// 创建数据库目录（如果需要）
static int create_database_directory(const char *path) {
    char dir_path[256];
    strncpy(dir_path, path, sizeof(dir_path) - 1);
    dir_path[sizeof(dir_path) - 1] = '\0';
    
    // 找到最后一个'/'的位置
    char *last_slash = strrchr(dir_path, '/');
    if (last_slash == NULL) {
        last_slash = strrchr(dir_path, '\\');
    }
    
    if (last_slash != NULL) {
        *last_slash = '\0'; // 截断路径，只保留目录部分
        
        // 检查目录是否存在
        struct stat st;
        if (stat(dir_path, &st) != 0) {
            // 目录不存在，尝试创建
            #ifdef _WIN32
            if (mkdir(dir_path) != 0) {
            #else
            if (mkdir(dir_path, 0755) != 0) {
            #endif
                fprintf(stderr, "创建数据库目录失败: %s\n", dir_path);
                return -1;
            }
            printf("数据库目录创建成功: %s\n", dir_path);
        }
    }
    
    return 0;
}

// 初始化数据库连接
int init_database() {
    // 检查数据库文件是否存在
    int db_exists = database_file_exists(DATABASE_PATH);
    
    // 创建数据库目录（如果需要）
    if (create_database_directory(DATABASE_PATH) != 0) {
        fprintf(stderr, "创建数据库目录失败\n");
        return -1;
    }
    
    // 打开数据库连接
    int rc = sqlite3_open(DATABASE_PATH, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "无法打开数据库: %s\n", sqlite3_errmsg(db));
        
        // 尝试重新创建数据库文件
        fprintf(stderr, "尝试重新创建数据库文件...\n");
        
        // 如果db指针不为NULL，先关闭
        if (db) {
            sqlite3_close(db);
            db = NULL;
        }
        
        // 删除损坏的数据库文件
        if (remove(DATABASE_PATH) == 0) {
            printf("已删除损坏的数据库文件\n");
        }
        
        // 重新尝试打开
        rc = sqlite3_open(DATABASE_PATH, &db);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "重新创建数据库失败: %s\n", sqlite3_errmsg(db));
            return -1;
        }
        
        printf("数据库文件重新创建成功\n");
    } else {
        if (db_exists) {
            printf("数据库连接成功（使用现有数据库）\n");
        } else {
            printf("数据库连接成功（创建新数据库）\n");
        }
    }
    
    // 启用外键约束
    rc = sqlite3_exec(db, "PRAGMA foreign_keys = ON;", 0, 0, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "启用外键约束失败: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        db = NULL;
        return -1;
    }
    
    // 如果数据库是新创建的，创建表结构
    if (!db_exists) {
        if (create_tables() != 0) {
            fprintf(stderr, "创建表结构失败\n");
            sqlite3_close(db);
            db = NULL;
            return -1;
        }
    } else {
        // 检查表是否存在，如果不存在则创建
        if (check_tables_exist() != 0) {
            fprintf(stderr, "检查表结构失败，尝试重新创建表...\n");
            if (create_tables() != 0) {
                fprintf(stderr, "重新创建表结构失败\n");
                sqlite3_close(db);
                db = NULL;
                return -1;
            }
        }
    }
    
    printf("数据库初始化完成\n");
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
        "variety VARCHAR(50) NOT NULL,"
        "planting_date DATE NOT NULL,"
        
        // 养护参数
        "water_frequency INTEGER,"
        "fertilize_frequency INTEGER,"
        "light_requirement VARCHAR(20),"
        "temperature_min INTEGER,"
        "temperature_max INTEGER,"
        "humidity_requirement VARCHAR(20),"
        
        // 状态跟踪
        "last_water_date DATE,"
        "last_fertilize_date DATE,"
        "last_pest_control_date DATE,"
        "status VARCHAR(20) DEFAULT '正常',"
        "health_score INTEGER DEFAULT 100,"
        
        // 统计分析字段
        "total_care_operations INTEGER DEFAULT 0,"
        "pest_incidents INTEGER DEFAULT 0,"
        
        "notes TEXT"
        ");";
    
    // 创建养护操作记录表
    const char *create_care_records_table = 
        "CREATE TABLE IF NOT EXISTS care_records ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "plant_id INTEGER NOT NULL,"
        
        // 操作类型细化
        "operation_category VARCHAR(20) NOT NULL,"
        "operation_type VARCHAR(30) NOT NULL,"
        
        "operation_date DATETIME NOT NULL,"
        
        // 操作详情
        "details TEXT,"
        "amount VARCHAR(20),"
        "duration_minutes INTEGER,"
        
        // 效果记录
        "plant_condition_before VARCHAR(20),"
        "plant_condition_after VARCHAR(20),"
        "effectiveness_rating INTEGER,"
        
        // 病虫害相关（如果是病虫害操作）
        "pest_type VARCHAR(30),"
        "control_method VARCHAR(30),"
        "pesticide_used VARCHAR(50),"
        
        // 环境条件
        "temperature INTEGER,"
        "humidity INTEGER,"
        "weather_condition VARCHAR(20),"
        
        "notes TEXT,"
        
        "FOREIGN KEY (plant_id) REFERENCES plants(id)"
        ");";
    
    // 创建养护提醒设置表
    const char *create_reminders_table = 
        "CREATE TABLE IF NOT EXISTS reminders ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "plant_id INTEGER NOT NULL,"
        
        "reminder_type VARCHAR(30) NOT NULL,"
        
        "frequency INTEGER NOT NULL,"
        "seasonal_adjustment BOOLEAN DEFAULT 0,"
        "spring_frequency INTEGER,"
        "summer_frequency INTEGER,"
        "autumn_frequency INTEGER,"
        "winter_frequency INTEGER,"
        
        "last_reminder_date DATE,"
        "next_reminder_date DATE,"
        "is_active BOOLEAN DEFAULT 1,"
        
        "priority INTEGER DEFAULT 1,"
        
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
    
    // 创建生长记录表
    const char *create_growth_records_table = 
        "CREATE TABLE IF NOT EXISTS growth_records ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "plant_id INTEGER NOT NULL,"
        "record_date DATE NOT NULL,"
        
        // 生长指标
        "height_cm DECIMAL(5,2),"
        "leaf_count INTEGER,"
        "bud_count INTEGER,"
        "flower_count INTEGER,"
        
        // 健康状况
        "health_score INTEGER,"
        "leaf_color VARCHAR(20),"
        "growth_vigor VARCHAR(20),"
        
        // 环境因素
        "temperature INTEGER,"
        "humidity INTEGER,"
        "light_exposure VARCHAR(20),"
        
        "notes TEXT,"
        "photo_path VARCHAR(200),"
        
        "FOREIGN KEY (plant_id) REFERENCES plants(id)"
        ");";
    
    rc = sqlite3_exec(db, create_growth_records_table, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "创建growth_records表失败: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }
    
    // 创建养护经验表
    const char *create_care_experience_table = 
        "CREATE TABLE IF NOT EXISTS care_experience ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "variety VARCHAR(50) NOT NULL,"
        
        // 成功经验
        "optimal_water_frequency INTEGER,"
        "optimal_fertilize_frequency INTEGER,"
        "best_season VARCHAR(20),"
        "common_pests TEXT,"
        "effective_controls TEXT,"
        
        // 失败教训
        "common_mistakes TEXT,"
        "warning_signs TEXT,"
        "recovery_methods TEXT,"
        
        // 统计分析
        "total_plants INTEGER DEFAULT 0,"
        "success_rate DECIMAL(5,2),"
        "avg_health_score DECIMAL(5,2),"
        
        "last_updated DATE,"
        "confidence_level INTEGER DEFAULT 1"
        ");";
    
    rc = sqlite3_exec(db, create_care_experience_table, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "创建care_experience表失败: %s\n", err_msg);
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

// 检查所有必需的表是否存在
int check_tables_exist() {
    const char* required_tables[] = {"plants", "care_records", "reminders", "growth_records", "care_experience"};
    int table_count = sizeof(required_tables) / sizeof(required_tables[0]);
    
    for (int i = 0; i < table_count; i++) {
        int exists = table_exists(required_tables[i]);
        if (exists == -1) {
            return -1; // 检查过程出错
        }
        if (exists == 0) {
            fprintf(stderr, "必需的表不存在: %s\n", required_tables[i]);
            return -1;
        }
    }
    
    printf("所有必需的表都存在\n");
    return 0;
}

// 手动创建数据库（独立函数，可在任何地方调用）
int create_database_manually() {
    printf("开始手动创建数据库...\n");
    
    // 检查并创建目录
    if (create_database_directory(DATABASE_PATH) != 0) {
        fprintf(stderr, "创建数据库目录失败\n");
        return -1;
    }
    
    // 如果数据库文件已存在，先备份
    if (database_file_exists(DATABASE_PATH)) {
        char backup_path[256];
        snprintf(backup_path, sizeof(backup_path), "%s.backup", DATABASE_PATH);
        
        if (rename(DATABASE_PATH, backup_path) == 0) {
            printf("已备份原有数据库文件: %s\n", backup_path);
        } else {
            fprintf(stderr, "备份数据库文件失败，继续创建新数据库\n");
        }
    }
    
    // 创建新的数据库连接
    sqlite3 *temp_db = NULL;
    int rc = sqlite3_open(DATABASE_PATH, &temp_db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "创建数据库文件失败: %s\n", sqlite3_errmsg(temp_db));
        if (temp_db) sqlite3_close(temp_db);
        return -1;
    }
    
    // 启用外键约束
    rc = sqlite3_exec(temp_db, "PRAGMA foreign_keys = ON;", 0, 0, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "启用外键约束失败: %s\n", sqlite3_errmsg(temp_db));
        sqlite3_close(temp_db);
        return -1;
    }
    
    // 创建表结构
    char *err_msg = NULL;
    const char* create_tables_sql[] = {
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
        ");",
        
        "CREATE TABLE IF NOT EXISTS care_records ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "plant_id INTEGER NOT NULL,"
        "operation_type VARCHAR(20) NOT NULL,"
        "operation_date DATETIME NOT NULL,"
        "details TEXT,"
        "amount VARCHAR(20),"
        "FOREIGN KEY (plant_id) REFERENCES plants(id)"
        ");",
        
        "CREATE TABLE IF NOT EXISTS reminders ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "plant_id INTEGER NOT NULL,"
        "reminder_type VARCHAR(20) NOT NULL,"
        "frequency INTEGER NOT NULL,"
        "last_reminder_date DATE,"
        "is_active BOOLEAN DEFAULT 1,"
        "FOREIGN KEY (plant_id) REFERENCES plants(id)"
        ");"
    };
    
    int table_count = sizeof(create_tables_sql) / sizeof(create_tables_sql[0]);
    
    for (int i = 0; i < table_count; i++) {
        rc = sqlite3_exec(temp_db, create_tables_sql[i], 0, 0, &err_msg);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "创建表失败: %s\n", err_msg);
            sqlite3_free(err_msg);
            sqlite3_close(temp_db);
            return -1;
        }
    }
    
    sqlite3_close(temp_db);
    printf("手动创建数据库成功: %s\n", DATABASE_PATH);
    return 0;
}

// 重置数据库（删除并重新创建）
int reset_database() {
    printf("开始重置数据库...\n");
    
    // 关闭现有连接
    if (db) {
        sqlite3_close(db);
        db = NULL;
    }
    
    // 等待一小段时间确保文件已释放
    #ifdef _WIN32
    Sleep(100);
    #else
    usleep(100000); // 100ms
    #endif
    
    // 删除数据库文件
    if (database_file_exists(DATABASE_PATH)) {
        int retry_count = 3;
        int delete_success = 0;
        
        for (int i = 0; i < retry_count; i++) {
            if (remove(DATABASE_PATH) == 0) {
                printf("已删除数据库文件\n");
                delete_success = 1;
                break;
            } else {
                fprintf(stderr, "删除数据库文件失败，重试 %d/%d\n", i + 1, retry_count);
                
                // 等待后重试
                #ifdef _WIN32
                Sleep(100);
                #else
                usleep(100000); // 100ms
                #endif
            }
        }
        
        if (!delete_success) {
            fprintf(stderr, "删除数据库文件失败，尝试强制重置\n");
            // 即使删除失败，也尝试继续创建新数据库
        }
    }
    
    // 重新创建数据库
    return create_database_manually();
}