#include "plants_dao.h"
#include <stdio.h>
#include <string.h>

// 初始化Plant结构体
void plant_init(Plant* plant) {
    if (!plant) return;
    
    memset(plant, 0, sizeof(Plant));
    plant->id = -1;
    plant->water_frequency = 7; // 默认7天浇水一次
    plant->fertilize_frequency = 30; // 默认30天施肥一次
    plant->health_score = 100; // 默认健康评分100
    plant->temperature_min = 15; // 默认最低温度15度
    plant->temperature_max = 30; // 默认最高温度30度
    strcpy(plant->status, "正常");
    strcpy(plant->light_requirement, "中"); // 默认中等光照
    strcpy(plant->humidity_requirement, "中"); // 默认中等湿度
}

// 添加花卉信息
DAO_RESULT plants_dao_add(const Plant* plant) {
    DAO_CHECK_PARAM(plant != NULL);
    DAO_CHECK_PARAM(strlen(plant->name) > 0);
    
    char *err_msg = NULL;
    char sql[2048];
    
    snprintf(sql, sizeof(sql),
        "INSERT INTO plants (name, variety, planting_date, water_frequency, "
        "fertilize_frequency, light_requirement, temperature_min, temperature_max, "
        "humidity_requirement, last_water_date, last_fertilize_date, "
        "last_pest_control_date, status, health_score, total_care_operations, "
        "pest_incidents, notes) "
        "VALUES ('%s', '%s', '%s', %d, %d, '%s', %d, %d, '%s', '%s', '%s', '%s', '%s', %d, %d, %d, '%s')",
        plant->name, plant->variety, plant->planting_date, plant->water_frequency,
        plant->fertilize_frequency, plant->light_requirement, plant->temperature_min,
        plant->temperature_max, plant->humidity_requirement, plant->last_water_date,
        plant->last_fertilize_date, plant->last_pest_control_date, plant->status,
        plant->health_score, plant->total_care_operations, plant->pest_incidents, plant->notes);
    
    int rc = sqlite3_exec(db, sql, NULL, 0, &err_msg);
    if (rc != SQLITE_OK) {
        DAO_CHECK_SQL(rc, err_msg);
        return DAO_DB_ERROR;
    }
    
    return DAO_SUCCESS;
}

// 根据ID更新花卉信息
DAO_RESULT plants_dao_update(int plant_id, const Plant* plant) {
    DAO_CHECK_PARAM(plant_id > 0);
    DAO_CHECK_PARAM(plant != NULL);
    
    char *err_msg = NULL;
    char sql[2048];
    
    snprintf(sql, sizeof(sql),
        "UPDATE plants SET name='%s', variety='%s', planting_date='%s', "
        "water_frequency=%d, fertilize_frequency=%d, light_requirement='%s', "
        "temperature_min=%d, temperature_max=%d, humidity_requirement='%s', "
        "last_water_date='%s', last_fertilize_date='%s', last_pest_control_date='%s', "
        "status='%s', health_score=%d, total_care_operations=%d, pest_incidents=%d, "
        "notes='%s' WHERE id=%d",
        plant->name, plant->variety, plant->planting_date, plant->water_frequency,
        plant->fertilize_frequency, plant->light_requirement, plant->temperature_min,
        plant->temperature_max, plant->humidity_requirement, plant->last_water_date,
        plant->last_fertilize_date, plant->last_pest_control_date, plant->status,
        plant->health_score, plant->total_care_operations, plant->pest_incidents,
        plant->notes, plant_id);
    
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

// 根据ID删除花卉信息
DAO_RESULT plants_dao_delete(int plant_id) {
    DAO_CHECK_PARAM(plant_id > 0);
    
    char *err_msg = NULL;
    char sql[256];
    
    snprintf(sql, sizeof(sql), "DELETE FROM plants WHERE id=%d", plant_id);
    
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
static int plants_query_callback(void* data, int argc, char** argv, char** col_names) {
    Plant plant;
    plant_init(&plant);
    
    for (int i = 0; i < argc; i++) {
        if (argv[i] == NULL) continue;
        
        if (strcmp(col_names[i], "id") == 0) {
            plant.id = atoi(argv[i]);
        } else if (strcmp(col_names[i], "name") == 0) {
            dao_strncpy_safe(plant.name, argv[i], sizeof(plant.name));
        } else if (strcmp(col_names[i], "variety") == 0) {
            dao_strncpy_safe(plant.variety, argv[i], sizeof(plant.variety));
        } else if (strcmp(col_names[i], "planting_date") == 0) {
            dao_strncpy_safe(plant.planting_date, argv[i], sizeof(plant.planting_date));
        } else if (strcmp(col_names[i], "water_frequency") == 0) {
            plant.water_frequency = atoi(argv[i]);
        } else if (strcmp(col_names[i], "fertilize_frequency") == 0) {
            plant.fertilize_frequency = atoi(argv[i]);
        } else if (strcmp(col_names[i], "light_requirement") == 0) {
            dao_strncpy_safe(plant.light_requirement, argv[i], sizeof(plant.light_requirement));
        } else if (strcmp(col_names[i], "temperature_min") == 0) {
            plant.temperature_min = atoi(argv[i]);
        } else if (strcmp(col_names[i], "temperature_max") == 0) {
            plant.temperature_max = atoi(argv[i]);
        } else if (strcmp(col_names[i], "humidity_requirement") == 0) {
            dao_strncpy_safe(plant.humidity_requirement, argv[i], sizeof(plant.humidity_requirement));
        } else if (strcmp(col_names[i], "last_water_date") == 0) {
            dao_strncpy_safe(plant.last_water_date, argv[i], sizeof(plant.last_water_date));
        } else if (strcmp(col_names[i], "last_fertilize_date") == 0) {
            dao_strncpy_safe(plant.last_fertilize_date, argv[i], sizeof(plant.last_fertilize_date));
        } else if (strcmp(col_names[i], "last_pest_control_date") == 0) {
            dao_strncpy_safe(plant.last_pest_control_date, argv[i], sizeof(plant.last_pest_control_date));
        } else if (strcmp(col_names[i], "status") == 0) {
            dao_strncpy_safe(plant.status, argv[i], sizeof(plant.status));
        } else if (strcmp(col_names[i], "health_score") == 0) {
            plant.health_score = atoi(argv[i]);
        } else if (strcmp(col_names[i], "total_care_operations") == 0) {
            plant.total_care_operations = atoi(argv[i]);
        } else if (strcmp(col_names[i], "pest_incidents") == 0) {
            plant.pest_incidents = atoi(argv[i]);
        } else if (strcmp(col_names[i], "notes") == 0) {
            dao_strncpy_safe(plant.notes, argv[i], sizeof(plant.notes));
        }
    }
    
    // 调用用户回调函数
    void (*callback)(const Plant*) = data;
    if (callback) {
        callback(&plant);
    }
    
    return 0;
}

// 根据ID查询花卉信息的回调函数
static int plants_get_by_id_callback(void* data, int argc, char** argv, char** col_names) {
    Plant* plant = (Plant*)data;
    plant_init(plant);
    
    for (int i = 0; i < argc; i++) {
        if (argv[i] == NULL) continue;
        
        if (strcmp(col_names[i], "id") == 0) {
            plant->id = atoi(argv[i]);
        } else if (strcmp(col_names[i], "name") == 0) {
            dao_strncpy_safe(plant->name, argv[i], sizeof(plant->name));
        } else if (strcmp(col_names[i], "variety") == 0) {
            dao_strncpy_safe(plant->variety, argv[i], sizeof(plant->variety));
        } else if (strcmp(col_names[i], "planting_date") == 0) {
            dao_strncpy_safe(plant->planting_date, argv[i], sizeof(plant->planting_date));
        } else if (strcmp(col_names[i], "water_frequency") == 0) {
            plant->water_frequency = atoi(argv[i]);
        } else if (strcmp(col_names[i], "fertilize_frequency") == 0) {
            plant->fertilize_frequency = atoi(argv[i]);
        } else if (strcmp(col_names[i], "light_requirement") == 0) {
            dao_strncpy_safe(plant->light_requirement, argv[i], sizeof(plant->light_requirement));
        } else if (strcmp(col_names[i], "temperature_min") == 0) {
            plant->temperature_min = atoi(argv[i]);
        } else if (strcmp(col_names[i], "temperature_max") == 0) {
            plant->temperature_max = atoi(argv[i]);
        } else if (strcmp(col_names[i], "humidity_requirement") == 0) {
            dao_strncpy_safe(plant->humidity_requirement, argv[i], sizeof(plant->humidity_requirement));
        } else if (strcmp(col_names[i], "last_water_date") == 0) {
            dao_strncpy_safe(plant->last_water_date, argv[i], sizeof(plant->last_water_date));
        } else if (strcmp(col_names[i], "last_fertilize_date") == 0) {
            dao_strncpy_safe(plant->last_fertilize_date, argv[i], sizeof(plant->last_fertilize_date));
        } else if (strcmp(col_names[i], "last_pest_control_date") == 0) {
            dao_strncpy_safe(plant->last_pest_control_date, argv[i], sizeof(plant->last_pest_control_date));
        } else if (strcmp(col_names[i], "status") == 0) {
            dao_strncpy_safe(plant->status, argv[i], sizeof(plant->status));
        } else if (strcmp(col_names[i], "health_score") == 0) {
            plant->health_score = atoi(argv[i]);
        } else if (strcmp(col_names[i], "total_care_operations") == 0) {
            plant->total_care_operations = atoi(argv[i]);
        } else if (strcmp(col_names[i], "pest_incidents") == 0) {
            plant->pest_incidents = atoi(argv[i]);
        } else if (strcmp(col_names[i], "notes") == 0) {
            dao_strncpy_safe(plant->notes, argv[i], sizeof(plant->notes));
        }
    }
    
    return 0;
}

// 根据ID查询花卉信息
DAO_RESULT plants_dao_get_by_id(int plant_id, Plant* plant) {
    DAO_CHECK_PARAM(plant_id > 0);
    DAO_CHECK_PARAM(plant != NULL);
    
    char *err_msg = NULL;
    char sql[256];
    
    snprintf(sql, sizeof(sql), "SELECT * FROM plants WHERE id=%d", plant_id);
    
    int rc = sqlite3_exec(db, sql, plants_get_by_id_callback, plant, &err_msg);
    
    if (rc != SQLITE_OK) {
        DAO_CHECK_SQL(rc, err_msg);
        return DAO_DB_ERROR;
    }
    
    // 检查是否找到了记录
    if (plant->id == -1) {
        return DAO_NOT_FOUND;
    }
    
    return DAO_SUCCESS;
}

// 查询所有花卉信息
DAO_RESULT plants_dao_get_all(void (*callback)(const Plant* plant)) {
    char *err_msg = NULL;
    
    int rc = sqlite3_exec(db, "SELECT * FROM plants ORDER BY id", 
                         plants_query_callback, (void*)callback, &err_msg);
    
    if (rc != SQLITE_OK) {
        DAO_CHECK_SQL(rc, err_msg);
        return DAO_DB_ERROR;
    }
    
    return DAO_SUCCESS;
}

// 根据条件查询花卉信息
DAO_RESULT plants_dao_query(const PlantQuery* query, void (*callback)(const Plant* plant)) {
    DAO_CHECK_PARAM(query != NULL);
    
    char sql[512] = "SELECT * FROM plants WHERE 1=1";
    
    if (strlen(query->name) > 0) {
        strcat(sql, " AND name LIKE '%");
        strcat(sql, query->name);
        strcat(sql, "%'");
    }
    
    if (strlen(query->variety) > 0) {
        strcat(sql, " AND variety LIKE '%");
        strcat(sql, query->variety);
        strcat(sql, "%'");
    }
    
    if (strlen(query->status) > 0) {
        strcat(sql, " AND status='");
        strcat(sql, query->status);
        strcat(sql, "'");
    }
    
    strcat(sql, " ORDER BY id");
    
    char *err_msg = NULL;
    int rc = sqlite3_exec(db, sql, plants_query_callback, (void*)callback, &err_msg);
    
    if (rc != SQLITE_OK) {
        DAO_CHECK_SQL(rc, err_msg);
        return DAO_DB_ERROR;
    }
    
    return DAO_SUCCESS;
}

// 更新最后浇水日期
DAO_RESULT plants_dao_update_last_water(int plant_id, const char* date) {
    DAO_CHECK_PARAM(plant_id > 0);
    DAO_CHECK_PARAM(date != NULL);
    
    char *err_msg = NULL;
    char sql[256];
    
    snprintf(sql, sizeof(sql), 
             "UPDATE plants SET last_water_date='%s' WHERE id=%d", date, plant_id);
    
    int rc = sqlite3_exec(db, sql, NULL, 0, &err_msg);
    if (rc != SQLITE_OK) {
        DAO_CHECK_SQL(rc, err_msg);
        return DAO_DB_ERROR;
    }
    
    return DAO_SUCCESS;
}

// 更新最后施肥日期
DAO_RESULT plants_dao_update_last_fertilize(int plant_id, const char* date) {
    DAO_CHECK_PARAM(plant_id > 0);
    DAO_CHECK_PARAM(date != NULL);
    
    char *err_msg = NULL;
    char sql[256];
    
    snprintf(sql, sizeof(sql), 
             "UPDATE plants SET last_fertilize_date='%s' WHERE id=%d", date, plant_id);
    
    int rc = sqlite3_exec(db, sql, NULL, 0, &err_msg);
    if (rc != SQLITE_OK) {
        DAO_CHECK_SQL(rc, err_msg);
        return DAO_DB_ERROR;
    }
    
    return DAO_SUCCESS;
}

// 更新生长状态
DAO_RESULT plants_dao_update_status(int plant_id, const char* status) {
    DAO_CHECK_PARAM(plant_id > 0);
    DAO_CHECK_PARAM(status != NULL);
    
    char *err_msg = NULL;
    char sql[256];
    
    snprintf(sql, sizeof(sql), 
             "UPDATE plants SET status='%s' WHERE id=%d", status, plant_id);
    
    int rc = sqlite3_exec(db, sql, NULL, 0, &err_msg);
    if (rc != SQLITE_OK) {
        DAO_CHECK_SQL(rc, err_msg);
        return DAO_DB_ERROR;
    }
    
    return DAO_SUCCESS;
}