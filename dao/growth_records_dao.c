#include "growth_records_dao.h"
#include <stdio.h>
#include <string.h>

// 初始化GrowthRecord结构体
void growth_record_init(GrowthRecord* record) {
    if (!record) return;
    
    memset(record, 0, sizeof(GrowthRecord));
    record->id = -1;
    record->plant_id = -1;
    record->height_cm = 0.0;
    record->leaf_count = 0;
    record->bud_count = 0;
    record->flower_count = 0;
    record->health_score = 100;
    record->temperature = 0;
    record->humidity = 0;
    strcpy(record->leaf_color, "绿色");
    strcpy(record->growth_vigor, "良好");
    strcpy(record->light_exposure, "适中");
}

// 查询回调函数（用于批量查询）
static int growth_records_query_callback(void* data, int argc, char** argv, char** col_names) {
    GrowthRecord record;
    growth_record_init(&record);
    
    for (int i = 0; i < argc; i++) {
        if (strcmp(col_names[i], "id") == 0 && argv[i]) {
            record.id = atoi(argv[i]);
        } else if (strcmp(col_names[i], "plant_id") == 0 && argv[i]) {
            record.plant_id = atoi(argv[i]);
        } else if (strcmp(col_names[i], "record_date") == 0 && argv[i]) {
            strncpy(record.record_date, argv[i], sizeof(record.record_date) - 1);
        } else if (strcmp(col_names[i], "height_cm") == 0 && argv[i]) {
            record.height_cm = atof(argv[i]);
        } else if (strcmp(col_names[i], "leaf_count") == 0 && argv[i]) {
            record.leaf_count = atoi(argv[i]);
        } else if (strcmp(col_names[i], "bud_count") == 0 && argv[i]) {
            record.bud_count = atoi(argv[i]);
        } else if (strcmp(col_names[i], "flower_count") == 0 && argv[i]) {
            record.flower_count = atoi(argv[i]);
        } else if (strcmp(col_names[i], "health_score") == 0 && argv[i]) {
            record.health_score = atoi(argv[i]);
        } else if (strcmp(col_names[i], "leaf_color") == 0 && argv[i]) {
            strncpy(record.leaf_color, argv[i], sizeof(record.leaf_color) - 1);
        } else if (strcmp(col_names[i], "growth_vigor") == 0 && argv[i]) {
            strncpy(record.growth_vigor, argv[i], sizeof(record.growth_vigor) - 1);
        } else if (strcmp(col_names[i], "temperature") == 0 && argv[i]) {
            record.temperature = atoi(argv[i]);
        } else if (strcmp(col_names[i], "humidity") == 0 && argv[i]) {
            record.humidity = atoi(argv[i]);
        } else if (strcmp(col_names[i], "light_exposure") == 0 && argv[i]) {
            strncpy(record.light_exposure, argv[i], sizeof(record.light_exposure) - 1);
        } else if (strcmp(col_names[i], "notes") == 0 && argv[i]) {
            strncpy(record.notes, argv[i], sizeof(record.notes) - 1);
        } else if (strcmp(col_names[i], "photo_path") == 0 && argv[i]) {
            strncpy(record.photo_path, argv[i], sizeof(record.photo_path) - 1);
        }
    }
    
    // 调用回调函数
    void (*callback)(const GrowthRecord*) = (void (*)(const GrowthRecord*))data;
    if (callback) {
        callback(&record);
    }
    
    return 0;
}

// 单条记录查询回调函数
static int growth_record_single_callback(void* data, int argc, char** argv, char** col_names) {
    GrowthRecord* record = (GrowthRecord*)data;
    growth_record_init(record);
    
    for (int i = 0; i < argc; i++) {
        if (strcmp(col_names[i], "id") == 0 && argv[i]) {
            record->id = atoi(argv[i]);
        } else if (strcmp(col_names[i], "plant_id") == 0 && argv[i]) {
            record->plant_id = atoi(argv[i]);
        } else if (strcmp(col_names[i], "record_date") == 0 && argv[i]) {
            strncpy(record->record_date, argv[i], sizeof(record->record_date) - 1);
        } else if (strcmp(col_names[i], "height_cm") == 0 && argv[i]) {
            record->height_cm = atof(argv[i]);
        } else if (strcmp(col_names[i], "leaf_count") == 0 && argv[i]) {
            record->leaf_count = atoi(argv[i]);
        } else if (strcmp(col_names[i], "bud_count") == 0 && argv[i]) {
            record->bud_count = atoi(argv[i]);
        } else if (strcmp(col_names[i], "flower_count") == 0 && argv[i]) {
            record->flower_count = atoi(argv[i]);
        } else if (strcmp(col_names[i], "health_score") == 0 && argv[i]) {
            record->health_score = atoi(argv[i]);
        } else if (strcmp(col_names[i], "leaf_color") == 0 && argv[i]) {
            strncpy(record->leaf_color, argv[i], sizeof(record->leaf_color) - 1);
        } else if (strcmp(col_names[i], "growth_vigor") == 0 && argv[i]) {
            strncpy(record->growth_vigor, argv[i], sizeof(record->growth_vigor) - 1);
        } else if (strcmp(col_names[i], "temperature") == 0 && argv[i]) {
            record->temperature = atoi(argv[i]);
        } else if (strcmp(col_names[i], "humidity") == 0 && argv[i]) {
            record->humidity = atoi(argv[i]);
        } else if (strcmp(col_names[i], "light_exposure") == 0 && argv[i]) {
            strncpy(record->light_exposure, argv[i], sizeof(record->light_exposure) - 1);
        } else if (strcmp(col_names[i], "notes") == 0 && argv[i]) {
            strncpy(record->notes, argv[i], sizeof(record->notes) - 1);
        } else if (strcmp(col_names[i], "photo_path") == 0 && argv[i]) {
            strncpy(record->photo_path, argv[i], sizeof(record->photo_path) - 1);
        }
    }
    
    return 0;
}

// 添加生长记录
DAO_RESULT growth_records_dao_add(const GrowthRecord* record) {
    DAO_CHECK_PARAM(record != NULL);
    DAO_CHECK_PARAM(record->plant_id > 0);
    DAO_CHECK_PARAM(strlen(record->record_date) > 0);
    
    char *err_msg = NULL;
    char sql[2048];
    
    snprintf(sql, sizeof(sql),
        "INSERT INTO growth_records (plant_id, record_date, height_cm, leaf_count, "
        "bud_count, flower_count, health_score, leaf_color, growth_vigor, "
        "temperature, humidity, light_exposure, notes, photo_path) "
        "VALUES (%d, '%s', %.2f, %d, %d, %d, %d, '%s', '%s', %d, %d, '%s', '%s', '%s')",
        record->plant_id, record->record_date, record->height_cm, record->leaf_count,
        record->bud_count, record->flower_count, record->health_score, record->leaf_color,
        record->growth_vigor, record->temperature, record->humidity, record->light_exposure,
        record->notes, record->photo_path);
    
    int rc = sqlite3_exec(db, sql, NULL, 0, &err_msg);
    if (rc != SQLITE_OK) {
        DAO_CHECK_SQL(rc, err_msg);
        return DAO_DB_ERROR;
    }
    
    return DAO_SUCCESS;
}

// 根据ID更新生长记录
DAO_RESULT growth_records_dao_update(int record_id, const GrowthRecord* record) {
    DAO_CHECK_PARAM(record_id > 0);
    DAO_CHECK_PARAM(record != NULL);
    DAO_CHECK_PARAM(record->plant_id > 0);
    
    char *err_msg = NULL;
    char sql[2048];
    
    snprintf(sql, sizeof(sql),
        "UPDATE growth_records SET plant_id=%d, record_date='%s', height_cm=%.2f, "
        "leaf_count=%d, bud_count=%d, flower_count=%d, health_score=%d, "
        "leaf_color='%s', growth_vigor='%s', temperature=%d, humidity=%d, "
        "light_exposure='%s', notes='%s', photo_path='%s' WHERE id=%d",
        record->plant_id, record->record_date, record->height_cm, record->leaf_count,
        record->bud_count, record->flower_count, record->health_score, record->leaf_color,
        record->growth_vigor, record->temperature, record->humidity, record->light_exposure,
        record->notes, record->photo_path, record_id);
    
    int rc = sqlite3_exec(db, sql, NULL, 0, &err_msg);
    if (rc != SQLITE_OK) {
        DAO_CHECK_SQL(rc, err_msg);
        return DAO_DB_ERROR;
    }
    
    // 检查是否真的更新了记录
    if (sqlite3_changes(db) == 0) {
        return DAO_NOT_FOUND;
    }
    
    return DAO_SUCCESS;
}

// 根据ID删除生长记录
DAO_RESULT growth_records_dao_delete(int record_id) {
    DAO_CHECK_PARAM(record_id > 0);
    
    char *err_msg = NULL;
    char sql[256];
    
    snprintf(sql, sizeof(sql), "DELETE FROM growth_records WHERE id=%d", record_id);
    
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

// 根据ID查询生长记录
DAO_RESULT growth_records_dao_get_by_id(int record_id, GrowthRecord* record) {
    DAO_CHECK_PARAM(record_id > 0);
    DAO_CHECK_PARAM(record != NULL);
    
    char *err_msg = NULL;
    char sql[256];
    
    snprintf(sql, sizeof(sql), "SELECT * FROM growth_records WHERE id=%d", record_id);
    
    int rc = sqlite3_exec(db, sql, growth_record_single_callback, record, &err_msg);
    
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

// 查询花卉的所有生长记录
DAO_RESULT growth_records_dao_get_by_plant(int plant_id, void (*callback)(const GrowthRecord* record)) {
    DAO_CHECK_PARAM(plant_id > 0);
    
    char *err_msg = NULL;
    char sql[256];
    
    snprintf(sql, sizeof(sql), "SELECT * FROM growth_records WHERE plant_id=%d ORDER BY record_date DESC", plant_id);
    
    int rc = sqlite3_exec(db, sql, growth_records_query_callback, (void*)callback, &err_msg);
    
    if (rc != SQLITE_OK) {
        DAO_CHECK_SQL(rc, err_msg);
        return DAO_DB_ERROR;
    }
    
    return DAO_SUCCESS;
}

// 查询所有生长记录
DAO_RESULT growth_records_dao_get_all(void (*callback)(const GrowthRecord* record)) {
    char *err_msg = NULL;
    
    int rc = sqlite3_exec(db, "SELECT * FROM growth_records ORDER BY record_date DESC", 
                         growth_records_query_callback, (void*)callback, &err_msg);
    
    if (rc != SQLITE_OK) {
        DAO_CHECK_SQL(rc, err_msg);
        return DAO_DB_ERROR;
    }
    
    return DAO_SUCCESS;
}

// 根据条件查询生长记录
DAO_RESULT growth_records_dao_query(const GrowthRecordQuery* query, void (*callback)(const GrowthRecord* record)) {
    DAO_CHECK_PARAM(query != NULL);
    
    char sql[512] = "SELECT * FROM growth_records WHERE 1=1";
    
    if (query->plant_id > 0) {
        char temp[50];
        snprintf(temp, sizeof(temp), " AND plant_id=%d", query->plant_id);
        strcat(sql, temp);
    }
    
    if (strlen(query->start_date) > 0) {
        char temp[100];
        snprintf(temp, sizeof(temp), " AND record_date >= '%s'", query->start_date);
        strcat(sql, temp);
    }
    
    if (strlen(query->end_date) > 0) {
        char temp[100];
        snprintf(temp, sizeof(temp), " AND record_date <= '%s'", query->end_date);
        strcat(sql, temp);
    }
    
    strcat(sql, " ORDER BY record_date DESC");
    
    char *err_msg = NULL;
    int rc = sqlite3_exec(db, sql, growth_records_query_callback, (void*)callback, &err_msg);
    
    if (rc != SQLITE_OK) {
        DAO_CHECK_SQL(rc, err_msg);
        return DAO_DB_ERROR;
    }
    
    return DAO_SUCCESS;
}

// 获取花卉的生长趋势数据
DAO_RESULT growth_records_dao_get_trend(int plant_id, GrowthRecord* earliest, GrowthRecord* latest) {
    DAO_CHECK_PARAM(plant_id > 0);
    DAO_CHECK_PARAM(earliest != NULL);
    DAO_CHECK_PARAM(latest != NULL);
    
    char *err_msg = NULL;
    
    // 获取最早的记录
    char sql_earliest[256];
    snprintf(sql_earliest, sizeof(sql_earliest), 
             "SELECT * FROM growth_records WHERE plant_id=%d ORDER BY record_date ASC LIMIT 1", plant_id);
    
    int rc = sqlite3_exec(db, sql_earliest, growth_records_query_callback, earliest, &err_msg);
    if (rc != SQLITE_OK) {
        DAO_CHECK_SQL(rc, err_msg);
        return DAO_DB_ERROR;
    }
    
    // 获取最新的记录
    char sql_latest[256];
    snprintf(sql_latest, sizeof(sql_latest), 
             "SELECT * FROM growth_records WHERE plant_id=%d ORDER BY record_date DESC LIMIT 1", plant_id);
    
    rc = sqlite3_exec(db, sql_latest, growth_records_query_callback, latest, &err_msg);
    if (rc != SQLITE_OK) {
        DAO_CHECK_SQL(rc, err_msg);
        return DAO_DB_ERROR;
    }
    
    return DAO_SUCCESS;
}