#include "care_records_dao.h"
#include <stdio.h>
#include <string.h>

// 初始化CareRecord结构体
void care_record_init(CareRecord* record) {
    if (!record) return;
    
    memset(record, 0, sizeof(CareRecord));
    record->id = -1;
    record->plant_id = -1;
    record->operation_type = CARE_OTHER;
    record->duration_minutes = 0;
    record->effectiveness_rating = 0;
    record->temperature = 0;
    record->humidity = 0;
    strcpy(record->operation_category, "常规");
    strcpy(record->plant_condition_before, "正常");
    strcpy(record->plant_condition_after, "正常");
    strcpy(record->weather_condition, "晴");
}

// 将操作类型枚举转换为字符串
const char* care_type_to_string(CareType type) {
    switch (type) {
        case CARE_WATER: return "浇水";
        case CARE_FERTILIZE: return "施肥";
        case CARE_PEST_CONTROL: return "病虫害防治";
        case CARE_OTHER: return "其他";
        default: return "未知";
    }
}

// 将字符串转换为操作类型枚举
CareType string_to_care_type(const char* type_str) {
    if (strcmp(type_str, "浇水") == 0) return CARE_WATER;
    if (strcmp(type_str, "施肥") == 0) return CARE_FERTILIZE;
    if (strcmp(type_str, "病虫害防治") == 0) return CARE_PEST_CONTROL;
    if (strcmp(type_str, "其他") == 0) return CARE_OTHER;
    return CARE_OTHER;
}

// 添加养护记录
DAO_RESULT care_records_dao_add(const CareRecord* record) {
    DAO_CHECK_PARAM(record != NULL);
    DAO_CHECK_PARAM(record->plant_id > 0);
    
    char *err_msg = NULL;
    char sql[2048];
    
    const char* type_str = care_type_to_string(record->operation_type);
    
    snprintf(sql, sizeof(sql),
        "INSERT INTO care_records (plant_id, operation_category, operation_type, operation_date, "
        "details, amount, duration_minutes, plant_condition_before, plant_condition_after, "
        "effectiveness_rating, pest_type, control_method, pesticide_used, temperature, "
        "humidity, weather_condition, notes) "
        "VALUES (%d, '%s', '%s', '%s', '%s', '%s', %d, '%s', '%s', %d, '%s', '%s', '%s', %d, %d, '%s', '%s')",
        record->plant_id, record->operation_category, type_str, record->operation_date, 
        record->details, record->amount, record->duration_minutes, record->plant_condition_before,
        record->plant_condition_after, record->effectiveness_rating, record->pest_type,
        record->control_method, record->pesticide_used, record->temperature, record->humidity,
        record->weather_condition, record->notes);
    
    int rc = sqlite3_exec(db, sql, NULL, 0, &err_msg);
    if (rc != SQLITE_OK) {
        DAO_CHECK_SQL(rc, err_msg);
        return DAO_DB_ERROR;
    }
    
    return DAO_SUCCESS;
}

// 根据ID删除养护记录
DAO_RESULT care_records_dao_delete(int record_id) {
    DAO_CHECK_PARAM(record_id > 0);
    
    char *err_msg = NULL;
    char sql[256];
    
    snprintf(sql, sizeof(sql), "DELETE FROM care_records WHERE id=%d", record_id);
    
    int rc = sqlite3_exec(db, sql, NULL, 0, &err_msg);
    if (rc != SQLITE_OK) {
        DAO_CHECK_SQL(rc, err_msg);
        return DAO_DB_ERROR;
    }
    
    // 检查是否真的删除了记录
    if (sqlite3_changes(db) == 0) {
        return DAO_NOT_FOUND;
    }
    
    return DAO_SUCCESS;
}

// 查询回调函数
static int care_records_query_callback(void* data, int argc, char** argv, char** col_names) {
    CareRecord record;
    care_record_init(&record);
    
    for (int i = 0; i < argc; i++) {
        if (argv[i] == NULL) continue;
        
        if (strcmp(col_names[i], "id") == 0) {
            record.id = atoi(argv[i]);
        } else if (strcmp(col_names[i], "plant_id") == 0) {
            record.plant_id = atoi(argv[i]);
        } else if (strcmp(col_names[i], "operation_category") == 0) {
            dao_strncpy_safe(record.operation_category, argv[i], sizeof(record.operation_category));
        } else if (strcmp(col_names[i], "operation_type") == 0) {
            record.operation_type = string_to_care_type(argv[i]);
        } else if (strcmp(col_names[i], "operation_date") == 0) {
            dao_strncpy_safe(record.operation_date, argv[i], sizeof(record.operation_date));
        } else if (strcmp(col_names[i], "details") == 0) {
            dao_strncpy_safe(record.details, argv[i], sizeof(record.details));
        } else if (strcmp(col_names[i], "amount") == 0) {
            dao_strncpy_safe(record.amount, argv[i], sizeof(record.amount));
        } else if (strcmp(col_names[i], "duration_minutes") == 0) {
            record.duration_minutes = atoi(argv[i]);
        } else if (strcmp(col_names[i], "plant_condition_before") == 0) {
            dao_strncpy_safe(record.plant_condition_before, argv[i], sizeof(record.plant_condition_before));
        } else if (strcmp(col_names[i], "plant_condition_after") == 0) {
            dao_strncpy_safe(record.plant_condition_after, argv[i], sizeof(record.plant_condition_after));
        } else if (strcmp(col_names[i], "effectiveness_rating") == 0) {
            record.effectiveness_rating = atoi(argv[i]);
        } else if (strcmp(col_names[i], "pest_type") == 0) {
            dao_strncpy_safe(record.pest_type, argv[i], sizeof(record.pest_type));
        } else if (strcmp(col_names[i], "control_method") == 0) {
            dao_strncpy_safe(record.control_method, argv[i], sizeof(record.control_method));
        } else if (strcmp(col_names[i], "pesticide_used") == 0) {
            dao_strncpy_safe(record.pesticide_used, argv[i], sizeof(record.pesticide_used));
        } else if (strcmp(col_names[i], "temperature") == 0) {
            record.temperature = atoi(argv[i]);
        } else if (strcmp(col_names[i], "humidity") == 0) {
            record.humidity = atoi(argv[i]);
        } else if (strcmp(col_names[i], "weather_condition") == 0) {
            dao_strncpy_safe(record.weather_condition, argv[i], sizeof(record.weather_condition));
        } else if (strcmp(col_names[i], "notes") == 0) {
            dao_strncpy_safe(record.notes, argv[i], sizeof(record.notes));
        }
    }
    
    // 调用用户回调函数
    void (*callback)(const CareRecord*) = data;
    if (callback) {
        callback(&record);
    }
    
    return 0;
}

// 根据ID查询单条记录的回调函数
static int care_records_get_by_id_callback(void* data, int argc, char** argv, char** col_names) {
    CareRecord* record = (CareRecord*)data;
    
    // 如果没有记录，保持id为-1
    if (argc == 0) {
        return 0;
    }
    
    for (int i = 0; i < argc; i++) {
        if (argv[i] == NULL) continue;
        
        if (strcmp(col_names[i], "id") == 0) {
            record->id = atoi(argv[i]);
        } else if (strcmp(col_names[i], "plant_id") == 0) {
            record->plant_id = atoi(argv[i]);
        } else if (strcmp(col_names[i], "operation_category") == 0) {
            dao_strncpy_safe(record->operation_category, argv[i], sizeof(record->operation_category));
        } else if (strcmp(col_names[i], "operation_type") == 0) {
            record->operation_type = string_to_care_type(argv[i]);
        } else if (strcmp(col_names[i], "operation_date") == 0) {
            dao_strncpy_safe(record->operation_date, argv[i], sizeof(record->operation_date));
        } else if (strcmp(col_names[i], "details") == 0) {
            dao_strncpy_safe(record->details, argv[i], sizeof(record->details));
        } else if (strcmp(col_names[i], "amount") == 0) {
            dao_strncpy_safe(record->amount, argv[i], sizeof(record->amount));
        } else if (strcmp(col_names[i], "duration_minutes") == 0) {
            record->duration_minutes = atoi(argv[i]);
        } else if (strcmp(col_names[i], "plant_condition_before") == 0) {
            dao_strncpy_safe(record->plant_condition_before, argv[i], sizeof(record->plant_condition_before));
        } else if (strcmp(col_names[i], "plant_condition_after") == 0) {
            dao_strncpy_safe(record->plant_condition_after, argv[i], sizeof(record->plant_condition_after));
        } else if (strcmp(col_names[i], "effectiveness_rating") == 0) {
            record->effectiveness_rating = atoi(argv[i]);
        } else if (strcmp(col_names[i], "pest_type") == 0) {
            dao_strncpy_safe(record->pest_type, argv[i], sizeof(record->pest_type));
        } else if (strcmp(col_names[i], "control_method") == 0) {
            dao_strncpy_safe(record->control_method, argv[i], sizeof(record->control_method));
        } else if (strcmp(col_names[i], "pesticide_used") == 0) {
            dao_strncpy_safe(record->pesticide_used, argv[i], sizeof(record->pesticide_used));
        } else if (strcmp(col_names[i], "temperature") == 0) {
            record->temperature = atoi(argv[i]);
        } else if (strcmp(col_names[i], "humidity") == 0) {
            record->humidity = atoi(argv[i]);
        } else if (strcmp(col_names[i], "weather_condition") == 0) {
            dao_strncpy_safe(record->weather_condition, argv[i], sizeof(record->weather_condition));
        } else if (strcmp(col_names[i], "notes") == 0) {
            dao_strncpy_safe(record->notes, argv[i], sizeof(record->notes));
        }
    }
    
    return 0;
}

// 根据ID查询养护记录
DAO_RESULT care_records_dao_get_by_id(int record_id, CareRecord* record) {
    DAO_CHECK_PARAM(record_id > 0);
    DAO_CHECK_PARAM(record != NULL);
    
    // 初始化记录
    care_record_init(record);
    
    char *err_msg = NULL;
    char sql[256];
    
    snprintf(sql, sizeof(sql), "SELECT * FROM care_records WHERE id=%d", record_id);
    
    int rc = sqlite3_exec(db, sql, care_records_get_by_id_callback, record, &err_msg);
    
    if (rc != SQLITE_OK) {
        DAO_CHECK_SQL(rc, err_msg);
        return DAO_DB_ERROR;
    }
    
    // 检查是否找到了记录
    if (record->id == -1) {
        return DAO_NOT_FOUND;
    }
    
    return DAO_SUCCESS;
}

// 查询花卉的所有养护记录
DAO_RESULT care_records_dao_get_by_plant(int plant_id, void (*callback)(const CareRecord* record)) {
    DAO_CHECK_PARAM(plant_id > 0);
    
    char *err_msg = NULL;
    char sql[256];
    
    snprintf(sql, sizeof(sql), 
             "SELECT * FROM care_records WHERE plant_id=%d ORDER BY operation_date DESC", 
             plant_id);
    
    int rc = sqlite3_exec(db, sql, care_records_query_callback, (void*)callback, &err_msg);
    
    if (rc != SQLITE_OK) {
        DAO_CHECK_SQL(rc, err_msg);
        return DAO_DB_ERROR;
    }
    
    return DAO_SUCCESS;
}

// 根据条件查询养护记录
DAO_RESULT care_records_dao_query(const CareRecordQuery* query, void (*callback)(const CareRecord* record)) {
    DAO_CHECK_PARAM(query != NULL);
    
    char sql[512] = "SELECT * FROM care_records WHERE 1=1";
    
    if (query->plant_id > 0) {
        char temp[20];
        snprintf(temp, sizeof(temp), " AND plant_id=%d", query->plant_id);
        strcat(sql, temp);
    }
    
    if (query->operation_type != CARE_OTHER) {
        const char* type_str = care_type_to_string(query->operation_type);
        strcat(sql, " AND operation_type='");
        strcat(sql, type_str);
        strcat(sql, "'");
    }
    
    if (strlen(query->start_date) > 0) {
        strcat(sql, " AND operation_date >= '");
        strcat(sql, query->start_date);
        strcat(sql, " 00:00:00'");
    }
    
    if (strlen(query->end_date) > 0) {
        strcat(sql, " AND operation_date <= '");
        strcat(sql, query->end_date);
        strcat(sql, " 23:59:59'");
    }
    
    strcat(sql, " ORDER BY operation_date DESC");
    
    char *err_msg = NULL;
    int rc = sqlite3_exec(db, sql, care_records_query_callback, (void*)callback, &err_msg);
    
    if (rc != SQLITE_OK) {
        DAO_CHECK_SQL(rc, err_msg);
        return DAO_DB_ERROR;
    }
    
    return DAO_SUCCESS;
}

// 统计回调函数
static int care_records_count_callback(void* data, int argc, char** argv, char** col_names) {
    if (argc > 0 && argv[0]) {
        *(int*)data = atoi(argv[0]);
    }
    return 0;
}

// 统计某花卉的养护次数
DAO_RESULT care_records_dao_count_by_plant(int plant_id, CareType type, int* count) {
    DAO_CHECK_PARAM(plant_id > 0);
    DAO_CHECK_PARAM(count != NULL);
    
    char *err_msg = NULL;
    char sql[256];
    
    if (type == CARE_OTHER) {
        snprintf(sql, sizeof(sql), 
                 "SELECT COUNT(*) FROM care_records WHERE plant_id=%d", plant_id);
    } else {
        const char* type_str = care_type_to_string(type);
        snprintf(sql, sizeof(sql), 
                 "SELECT COUNT(*) FROM care_records WHERE plant_id=%d AND operation_type='%s'", 
                 plant_id, type_str);
    }
    
    *count = 0;
    int rc = sqlite3_exec(db, sql, care_records_count_callback, count, &err_msg);
    
    if (rc != SQLITE_OK) {
        DAO_CHECK_SQL(rc, err_msg);
        return DAO_DB_ERROR;
    }
    
    return DAO_SUCCESS;
}

// 获取最近一次养护记录
DAO_RESULT care_records_dao_get_latest(int plant_id, CareType type, CareRecord* record) {
    DAO_CHECK_PARAM(plant_id > 0);
    DAO_CHECK_PARAM(record != NULL);
    
    char *err_msg = NULL;
    char sql[256];
    
    const char* type_str = care_type_to_string(type);
    snprintf(sql, sizeof(sql), 
             "SELECT * FROM care_records WHERE plant_id=%d AND operation_type='%s' "
             "ORDER BY operation_date DESC LIMIT 1", 
             plant_id, type_str);
    
    int rc = sqlite3_exec(db, sql, care_records_query_callback, record, &err_msg);
    
    if (rc != SQLITE_OK) {
        DAO_CHECK_SQL(rc, err_msg);
        return DAO_DB_ERROR;
    }
    
    // 检查是否找到了记录
    if (record->id == -1) {
        return DAO_NOT_FOUND;
    }
    
    return DAO_SUCCESS;
}