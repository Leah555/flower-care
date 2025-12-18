#include "care_experience_dao.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern sqlite3* db;

// 初始化养护经验结构体
void care_experience_init(CareExperience* experience) {
    if (experience == NULL) return;
    
    memset(experience, 0, sizeof(CareExperience));
    experience->id = -1;
    experience->optimal_water_frequency = 0;
    experience->optimal_fertilize_frequency = 0;
    experience->total_plants = 0;
    experience->success_rate = 0.0;
    experience->avg_health_score = 0.0;
    experience->confidence_level = 1;
}

// 添加养护经验
DAO_RESULT care_experience_dao_add(CareExperience* experience) {
    if (experience == NULL) {
        return DAO_ERROR;
    }
    
    char* err_msg = NULL;
    const char* sql = "INSERT INTO care_experience (variety, optimal_water_frequency, optimal_fertilize_frequency, "
                     "best_season, common_pests, effective_controls, common_mistakes, warning_signs, recovery_methods, "
                     "total_plants, success_rate, avg_health_score, last_updated, confidence_level) "
                     "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "准备插入语句失败: %s\n", sqlite3_errmsg(db));
        return DAO_ERROR;
    }
    
    // 绑定参数
    sqlite3_bind_text(stmt, 1, experience->variety, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, experience->optimal_water_frequency);
    sqlite3_bind_int(stmt, 3, experience->optimal_fertilize_frequency);
    sqlite3_bind_text(stmt, 4, experience->best_season, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, experience->common_pests, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, experience->effective_controls, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 7, experience->common_mistakes, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 8, experience->warning_signs, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 9, experience->recovery_methods, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 10, experience->total_plants);
    sqlite3_bind_double(stmt, 11, experience->success_rate);
    sqlite3_bind_double(stmt, 12, experience->avg_health_score);
    sqlite3_bind_text(stmt, 13, experience->last_updated, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 14, experience->confidence_level);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "插入养护经验失败: %s\n", sqlite3_errmsg(db));
        return DAO_ERROR;
    }
    
    // 获取插入的ID
    experience->id = (int)sqlite3_last_insert_rowid(db);
    return DAO_SUCCESS;
}

// 更新养护经验
DAO_RESULT care_experience_dao_update(int id, CareExperience* experience) {
    if (experience == NULL) {
        return DAO_ERROR;
    }
    
    char* err_msg = NULL;
    const char* sql = "UPDATE care_experience SET variety = ?, optimal_water_frequency = ?, optimal_fertilize_frequency = ?, "
                     "best_season = ?, common_pests = ?, effective_controls = ?, common_mistakes = ?, warning_signs = ?, "
                     "recovery_methods = ?, total_plants = ?, success_rate = ?, avg_health_score = ?, "
                     "last_updated = ?, confidence_level = ? WHERE id = ?";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "准备更新语句失败: %s\n", sqlite3_errmsg(db));
        return DAO_ERROR;
    }
    
    // 绑定参数
    sqlite3_bind_text(stmt, 1, experience->variety, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, experience->optimal_water_frequency);
    sqlite3_bind_int(stmt, 3, experience->optimal_fertilize_frequency);
    sqlite3_bind_text(stmt, 4, experience->best_season, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, experience->common_pests, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, experience->effective_controls, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 7, experience->common_mistakes, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 8, experience->warning_signs, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 9, experience->recovery_methods, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 10, experience->total_plants);
    sqlite3_bind_double(stmt, 11, experience->success_rate);
    sqlite3_bind_double(stmt, 12, experience->avg_health_score);
    sqlite3_bind_text(stmt, 13, experience->last_updated, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 14, experience->confidence_level);
    sqlite3_bind_int(stmt, 15, id);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "更新养护经验失败: %s\n", sqlite3_errmsg(db));
        return DAO_ERROR;
    }
    
    if (sqlite3_changes(db) == 0) {
        return DAO_NOT_FOUND;
    }
    
    return DAO_SUCCESS;
}

// 删除养护经验
DAO_RESULT care_experience_dao_delete(int id) {
    char* err_msg = NULL;
    const char* sql = "DELETE FROM care_experience WHERE id = ?";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "准备删除语句失败: %s\n", sqlite3_errmsg(db));
        return DAO_ERROR;
    }
    
    sqlite3_bind_int(stmt, 1, id);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "删除养护经验失败: %s\n", sqlite3_errmsg(db));
        return DAO_ERROR;
    }
    
    if (sqlite3_changes(db) == 0) {
        return DAO_NOT_FOUND;
    }
    
    return DAO_SUCCESS;
}

// 根据ID查询养护经验
DAO_RESULT care_experience_dao_get_by_id(int id, CareExperience* experience) {
    if (experience == NULL) {
        return DAO_ERROR;
    }
    
    char* err_msg = NULL;
    const char* sql = "SELECT * FROM care_experience WHERE id = ?";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "准备查询语句失败: %s\n", sqlite3_errmsg(db));
        return DAO_ERROR;
    }
    
    sqlite3_bind_int(stmt, 1, id);
    
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        // 填充结构体
        experience->id = sqlite3_column_int(stmt, 0);
        strcpy(experience->variety, (const char*)sqlite3_column_text(stmt, 1));
        experience->optimal_water_frequency = sqlite3_column_int(stmt, 2);
        experience->optimal_fertilize_frequency = sqlite3_column_int(stmt, 3);
        strcpy(experience->best_season, (const char*)sqlite3_column_text(stmt, 4));
        strcpy(experience->common_pests, (const char*)sqlite3_column_text(stmt, 5));
        strcpy(experience->effective_controls, (const char*)sqlite3_column_text(stmt, 6));
        strcpy(experience->common_mistakes, (const char*)sqlite3_column_text(stmt, 7));
        strcpy(experience->warning_signs, (const char*)sqlite3_column_text(stmt, 8));
        strcpy(experience->recovery_methods, (const char*)sqlite3_column_text(stmt, 9));
        experience->total_plants = sqlite3_column_int(stmt, 10);
        experience->success_rate = sqlite3_column_double(stmt, 11);
        experience->avg_health_score = sqlite3_column_double(stmt, 12);
        strcpy(experience->last_updated, (const char*)sqlite3_column_text(stmt, 13));
        experience->confidence_level = sqlite3_column_int(stmt, 14);
        
        sqlite3_finalize(stmt);
        return DAO_SUCCESS;
    } else if (rc == SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return DAO_NOT_FOUND;
    } else {
        fprintf(stderr, "查询养护经验失败: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return DAO_ERROR;
    }
}

// 查询所有养护经验
DAO_RESULT care_experience_dao_get_all(void (*callback)(const CareExperience*)) {
    if (callback == NULL) {
        return DAO_ERROR;
    }
    
    char* err_msg = NULL;
    const char* sql = "SELECT * FROM care_experience";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "准备查询语句失败: %s\n", sqlite3_errmsg(db));
        return DAO_ERROR;
    }
    
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        CareExperience experience;
        care_experience_init(&experience);
        
        // 填充结构体
        experience.id = sqlite3_column_int(stmt, 0);
        strcpy(experience.variety, (const char*)sqlite3_column_text(stmt, 1));
        experience.optimal_water_frequency = sqlite3_column_int(stmt, 2);
        experience.optimal_fertilize_frequency = sqlite3_column_int(stmt, 3);
        strcpy(experience.best_season, (const char*)sqlite3_column_text(stmt, 4));
        strcpy(experience.common_pests, (const char*)sqlite3_column_text(stmt, 5));
        strcpy(experience.effective_controls, (const char*)sqlite3_column_text(stmt, 6));
        strcpy(experience.common_mistakes, (const char*)sqlite3_column_text(stmt, 7));
        strcpy(experience.warning_signs, (const char*)sqlite3_column_text(stmt, 8));
        strcpy(experience.recovery_methods, (const char*)sqlite3_column_text(stmt, 9));
        experience.total_plants = sqlite3_column_int(stmt, 10);
        experience.success_rate = sqlite3_column_double(stmt, 11);
        experience.avg_health_score = sqlite3_column_double(stmt, 12);
        strcpy(experience.last_updated, (const char*)sqlite3_column_text(stmt, 13));
        experience.confidence_level = sqlite3_column_int(stmt, 14);
        
        callback(&experience);
    }
    
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "查询所有养护经验失败: %s\n", sqlite3_errmsg(db));
        return DAO_ERROR;
    }
    
    return DAO_SUCCESS;
}

// 条件查询养护经验
DAO_RESULT care_experience_dao_query(CareExperienceQuery* query, void (*callback)(const CareExperience*)) {
    if (query == NULL || callback == NULL) {
        return DAO_ERROR;
    }
    
    char* err_msg = NULL;
    const char* sql = "SELECT * FROM care_experience WHERE 1=1";
    
    // 构建动态SQL
    char dynamic_sql[1024];
    strcpy(dynamic_sql, sql);
    
    if (strlen(query->variety) > 0) {
        strcat(dynamic_sql, " AND variety LIKE ?");
    }
    if (strlen(query->best_season) > 0) {
        strcat(dynamic_sql, " AND best_season = ?");
    }
    if (query->min_confidence_level > 0) {
        strcat(dynamic_sql, " AND confidence_level >= ?");
    }
    if (query->max_confidence_level > 0) {
        strcat(dynamic_sql, " AND confidence_level <= ?");
    }
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, dynamic_sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "准备条件查询语句失败: %s\n", sqlite3_errmsg(db));
        return DAO_ERROR;
    }
    
    // 绑定参数
    int param_index = 1;
    if (strlen(query->variety) > 0) {
        char like_pattern[60];
        snprintf(like_pattern, sizeof(like_pattern), "%%%s%%", query->variety);
        sqlite3_bind_text(stmt, param_index++, like_pattern, -1, SQLITE_STATIC);
    }
    if (strlen(query->best_season) > 0) {
        sqlite3_bind_text(stmt, param_index++, query->best_season, -1, SQLITE_STATIC);
    }
    if (query->min_confidence_level > 0) {
        sqlite3_bind_int(stmt, param_index++, query->min_confidence_level);
    }
    if (query->max_confidence_level > 0) {
        sqlite3_bind_int(stmt, param_index++, query->max_confidence_level);
    }
    
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        CareExperience experience;
        care_experience_init(&experience);
        
        // 填充结构体
        experience.id = sqlite3_column_int(stmt, 0);
        strcpy(experience.variety, (const char*)sqlite3_column_text(stmt, 1));
        experience.optimal_water_frequency = sqlite3_column_int(stmt, 2);
        experience.optimal_fertilize_frequency = sqlite3_column_int(stmt, 3);
        strcpy(experience.best_season, (const char*)sqlite3_column_text(stmt, 4));
        strcpy(experience.common_pests, (const char*)sqlite3_column_text(stmt, 5));
        strcpy(experience.effective_controls, (const char*)sqlite3_column_text(stmt, 6));
        strcpy(experience.common_mistakes, (const char*)sqlite3_column_text(stmt, 7));
        strcpy(experience.warning_signs, (const char*)sqlite3_column_text(stmt, 8));
        strcpy(experience.recovery_methods, (const char*)sqlite3_column_text(stmt, 9));
        experience.total_plants = sqlite3_column_int(stmt, 10);
        experience.success_rate = sqlite3_column_double(stmt, 11);
        experience.avg_health_score = sqlite3_column_double(stmt, 12);
        strcpy(experience.last_updated, (const char*)sqlite3_column_text(stmt, 13));
        experience.confidence_level = sqlite3_column_int(stmt, 14);
        
        callback(&experience);
    }
    
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "条件查询养护经验失败: %s\n", sqlite3_errmsg(db));
        return DAO_ERROR;
    }
    
    return DAO_SUCCESS;
}