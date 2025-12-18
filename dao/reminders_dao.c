#include "reminders_dao.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

// 初始化Reminder结构体
void reminder_init(Reminder* reminder) {
    if (!reminder) return;
    
    memset(reminder, 0, sizeof(Reminder));
    reminder->id = -1;
    reminder->plant_id = -1;
    reminder->reminder_type = REMINDER_WATER;
    reminder->frequency = 7; // 默认7天
    reminder->seasonal_adjustment = 0; // 默认不启用季节调整
    reminder->spring_frequency = 7;
    reminder->summer_frequency = 5; // 夏季频率更高
    reminder->autumn_frequency = 7;
    reminder->winter_frequency = 10; // 冬季频率更低
    reminder->is_active = 1; // 默认激活
    reminder->priority = 1; // 默认优先级1
}

// 将提醒类型枚举转换为字符串
const char* reminder_type_to_string(ReminderType type) {
    switch (type) {
        case REMINDER_WATER: return "浇水";
        case REMINDER_FERTILIZE: return "施肥";
        default: return "未知";
    }
}

// 将字符串转换为提醒类型枚举
ReminderType string_to_reminder_type(const char* type_str) {
    if (strcmp(type_str, "浇水") == 0) return REMINDER_WATER;
    if (strcmp(type_str, "施肥") == 0) return REMINDER_FERTILIZE;
    return REMINDER_WATER;
}

// 添加提醒设置
DAO_RESULT reminders_dao_add(const Reminder* reminder) {
    DAO_CHECK_PARAM(reminder != NULL);
    DAO_CHECK_PARAM(reminder->plant_id > 0);
    DAO_CHECK_PARAM(reminder->frequency > 0);
    
    char *err_msg = NULL;
    char sql[2048];
    
    const char* type_str = reminder_type_to_string(reminder->reminder_type);
    
    snprintf(sql, sizeof(sql),
        "INSERT INTO reminders (plant_id, reminder_type, frequency, seasonal_adjustment, "
        "spring_frequency, summer_frequency, autumn_frequency, winter_frequency, "
        "last_reminder_date, next_reminder_date, is_active, priority) "
        "VALUES (%d, '%s', %d, %d, %d, %d, %d, %d, '%s', '%s', %d, %d)",
        reminder->plant_id, type_str, reminder->frequency, reminder->seasonal_adjustment,
        reminder->spring_frequency, reminder->summer_frequency, reminder->autumn_frequency,
        reminder->winter_frequency, reminder->last_reminder_date, reminder->next_reminder_date,
        reminder->is_active, reminder->priority);
    
    int rc = sqlite3_exec(db, sql, NULL, 0, &err_msg);
    if (rc != SQLITE_OK) {
        DAO_CHECK_SQL(rc, err_msg);
        return DAO_DB_ERROR;
    }
    
    return DAO_SUCCESS;
}

// 根据ID更新提醒设置
DAO_RESULT reminders_dao_update(int reminder_id, const Reminder* reminder) {
    DAO_CHECK_PARAM(reminder_id > 0);
    DAO_CHECK_PARAM(reminder != NULL);
    DAO_CHECK_PARAM(reminder->frequency > 0);
    
    char *err_msg = NULL;
    char sql[2048];
    
    const char* type_str = reminder_type_to_string(reminder->reminder_type);
    
    snprintf(sql, sizeof(sql),
        "UPDATE reminders SET plant_id=%d, reminder_type='%s', frequency=%d, "
        "seasonal_adjustment=%d, spring_frequency=%d, summer_frequency=%d, "
        "autumn_frequency=%d, winter_frequency=%d, last_reminder_date='%s', "
        "next_reminder_date='%s', is_active=%d, priority=%d WHERE id=%d",
        reminder->plant_id, type_str, reminder->frequency, reminder->seasonal_adjustment,
        reminder->spring_frequency, reminder->summer_frequency, reminder->autumn_frequency,
        reminder->winter_frequency, reminder->last_reminder_date, reminder->next_reminder_date,
        reminder->is_active, reminder->priority, reminder_id);
    
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

// 根据ID删除提醒设置
DAO_RESULT reminders_dao_delete(int reminder_id) {
    DAO_CHECK_PARAM(reminder_id > 0);
    
    char *err_msg = NULL;
    char sql[256];
    
    snprintf(sql, sizeof(sql), "DELETE FROM reminders WHERE id=%d", reminder_id);
    
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
static int reminders_query_callback(void* data, int argc, char** argv, char** col_names) {
    Reminder reminder;
    reminder_init(&reminder);
    
    for (int i = 0; i < argc; i++) {
        if (argv[i] == NULL) continue;
        
        if (strcmp(col_names[i], "id") == 0) {
            reminder.id = atoi(argv[i]);
        } else if (strcmp(col_names[i], "plant_id") == 0) {
            reminder.plant_id = atoi(argv[i]);
        } else if (strcmp(col_names[i], "reminder_type") == 0) {
            reminder.reminder_type = string_to_reminder_type(argv[i]);
        } else if (strcmp(col_names[i], "frequency") == 0) {
            reminder.frequency = atoi(argv[i]);
        } else if (strcmp(col_names[i], "seasonal_adjustment") == 0) {
            reminder.seasonal_adjustment = atoi(argv[i]);
        } else if (strcmp(col_names[i], "spring_frequency") == 0) {
            reminder.spring_frequency = atoi(argv[i]);
        } else if (strcmp(col_names[i], "summer_frequency") == 0) {
            reminder.summer_frequency = atoi(argv[i]);
        } else if (strcmp(col_names[i], "autumn_frequency") == 0) {
            reminder.autumn_frequency = atoi(argv[i]);
        } else if (strcmp(col_names[i], "winter_frequency") == 0) {
            reminder.winter_frequency = atoi(argv[i]);
        } else if (strcmp(col_names[i], "last_reminder_date") == 0) {
            dao_strncpy_safe(reminder.last_reminder_date, argv[i], sizeof(reminder.last_reminder_date));
        } else if (strcmp(col_names[i], "next_reminder_date") == 0) {
            dao_strncpy_safe(reminder.next_reminder_date, argv[i], sizeof(reminder.next_reminder_date));
        } else if (strcmp(col_names[i], "is_active") == 0) {
            reminder.is_active = atoi(argv[i]);
        } else if (strcmp(col_names[i], "priority") == 0) {
            reminder.priority = atoi(argv[i]);
        }
    }
    
    // 调用用户回调函数
    void (*callback)(const Reminder*) = data;
    if (callback) {
        callback(&reminder);
    }
    
    return 0;
}

// 根据ID查询单条提醒记录的回调函数
static int reminders_get_by_id_callback(void* data, int argc, char** argv, char** col_names) {
    Reminder* reminder = (Reminder*)data;
    
    // 如果没有记录，保持id为-1
    if (argc == 0) {
        return 0;
    }
    
    for (int i = 0; i < argc; i++) {
        if (argv[i] == NULL) continue;
        
        if (strcmp(col_names[i], "id") == 0) {
            reminder->id = atoi(argv[i]);
        } else if (strcmp(col_names[i], "plant_id") == 0) {
            reminder->plant_id = atoi(argv[i]);
        } else if (strcmp(col_names[i], "reminder_type") == 0) {
            reminder->reminder_type = string_to_reminder_type(argv[i]);
        } else if (strcmp(col_names[i], "frequency") == 0) {
            reminder->frequency = atoi(argv[i]);
        } else if (strcmp(col_names[i], "seasonal_adjustment") == 0) {
            reminder->seasonal_adjustment = atoi(argv[i]);
        } else if (strcmp(col_names[i], "spring_frequency") == 0) {
            reminder->spring_frequency = atoi(argv[i]);
        } else if (strcmp(col_names[i], "summer_frequency") == 0) {
            reminder->summer_frequency = atoi(argv[i]);
        } else if (strcmp(col_names[i], "autumn_frequency") == 0) {
            reminder->autumn_frequency = atoi(argv[i]);
        } else if (strcmp(col_names[i], "winter_frequency") == 0) {
            reminder->winter_frequency = atoi(argv[i]);
        } else if (strcmp(col_names[i], "last_reminder_date") == 0) {
            dao_strncpy_safe(reminder->last_reminder_date, argv[i], sizeof(reminder->last_reminder_date));
        } else if (strcmp(col_names[i], "next_reminder_date") == 0) {
            dao_strncpy_safe(reminder->next_reminder_date, argv[i], sizeof(reminder->next_reminder_date));
        } else if (strcmp(col_names[i], "is_active") == 0) {
            reminder->is_active = atoi(argv[i]);
        } else if (strcmp(col_names[i], "priority") == 0) {
            reminder->priority = atoi(argv[i]);
        }
    }
    
    return 0;
}

// 根据ID查询提醒设置
DAO_RESULT reminders_dao_get_by_id(int reminder_id, Reminder* reminder) {
    DAO_CHECK_PARAM(reminder_id > 0);
    DAO_CHECK_PARAM(reminder != NULL);
    
    // 初始化提醒
    reminder_init(reminder);
    
    char *err_msg = NULL;
    char sql[256];
    
    snprintf(sql, sizeof(sql), "SELECT * FROM reminders WHERE id=%d", reminder_id);
    
    int rc = sqlite3_exec(db, sql, reminders_get_by_id_callback, reminder, &err_msg);
    
    if (rc != SQLITE_OK) {
        DAO_CHECK_SQL(rc, err_msg);
        return DAO_DB_ERROR;
    }
    
    // 检查是否找到了记录
    if (reminder->id == -1) {
        return DAO_NOT_FOUND;
    }
    
    return DAO_SUCCESS;
}

// 查询花卉的所有提醒设置
DAO_RESULT reminders_dao_get_by_plant(int plant_id, void (*callback)(const Reminder* reminder)) {
    DAO_CHECK_PARAM(plant_id > 0);
    
    char *err_msg = NULL;
    char sql[256];
    
    snprintf(sql, sizeof(sql), 
             "SELECT * FROM reminders WHERE plant_id=%d ORDER BY reminder_type", 
             plant_id);
    
    int rc = sqlite3_exec(db, sql, reminders_query_callback, (void*)callback, &err_msg);
    
    if (rc != SQLITE_OK) {
        DAO_CHECK_SQL(rc, err_msg);
        return DAO_DB_ERROR;
    }
    
    return DAO_SUCCESS;
}

// 查询所有提醒设置
DAO_RESULT reminders_dao_get_all(void (*callback)(const Reminder* reminder)) {
    char *err_msg = NULL;
    
    int rc = sqlite3_exec(db, "SELECT * FROM reminders ORDER BY plant_id, reminder_type", 
                         reminders_query_callback, (void*)callback, &err_msg);
    
    if (rc != SQLITE_OK) {
        DAO_CHECK_SQL(rc, err_msg);
        return DAO_DB_ERROR;
    }
    
    return DAO_SUCCESS;
}

// 根据条件查询提醒设置
DAO_RESULT reminders_dao_query(const ReminderQuery* query, void (*callback)(const Reminder* reminder)) {
    DAO_CHECK_PARAM(query != NULL);
    
    char sql[512] = "SELECT * FROM reminders WHERE 1=1";
    
    if (query->plant_id > 0) {
        char temp[20];
        snprintf(temp, sizeof(temp), " AND plant_id=%d", query->plant_id);
        strcat(sql, temp);
    }
    
    if (query->reminder_type != REMINDER_WATER) { // 默认是浇水
        const char* type_str = reminder_type_to_string(query->reminder_type);
        strcat(sql, " AND reminder_type='");
        strcat(sql, type_str);
        strcat(sql, "'");
    }
    
    if (query->is_active >= 0) {
        char temp[20];
        snprintf(temp, sizeof(temp), " AND is_active=%d", query->is_active);
        strcat(sql, temp);
    }
    
    strcat(sql, " ORDER BY plant_id, reminder_type");
    
    char *err_msg = NULL;
    int rc = sqlite3_exec(db, sql, reminders_query_callback, (void*)callback, &err_msg);
    
    if (rc != SQLITE_OK) {
        DAO_CHECK_SQL(rc, err_msg);
        return DAO_DB_ERROR;
    }
    
    return DAO_SUCCESS;
}

// 更新最后提醒日期
DAO_RESULT reminders_dao_update_last_reminder(int reminder_id, const char* date) {
    DAO_CHECK_PARAM(reminder_id > 0);
    DAO_CHECK_PARAM(date != NULL);
    
    char *err_msg = NULL;
    char sql[256];
    
    snprintf(sql, sizeof(sql), 
             "UPDATE reminders SET last_reminder_date='%s' WHERE id=%d", date, reminder_id);
    
    int rc = sqlite3_exec(db, sql, NULL, 0, &err_msg);
    if (rc != SQLITE_OK) {
        DAO_CHECK_SQL(rc, err_msg);
        return DAO_DB_ERROR;
    }
    
    return DAO_SUCCESS;
}

// 激活/停用提醒
DAO_RESULT reminders_dao_set_active(int reminder_id, int is_active) {
    DAO_CHECK_PARAM(reminder_id > 0);
    DAO_CHECK_PARAM(is_active == 0 || is_active == 1);
    
    char *err_msg = NULL;
    char sql[256];
    
    snprintf(sql, sizeof(sql), 
             "UPDATE reminders SET is_active=%d WHERE id=%d", is_active, reminder_id);
    
    int rc = sqlite3_exec(db, sql, NULL, 0, &err_msg);
    if (rc != SQLITE_OK) {
        DAO_CHECK_SQL(rc, err_msg);
        return DAO_DB_ERROR;
    }
    
    return DAO_SUCCESS;
}

// 计算两个日期之间的天数差
static int days_between_dates(const char* date1, const char* date2) {
    struct tm tm1 = {0}, tm2 = {0};
    
    sscanf(date1, "%d-%d-%d", &tm1.tm_year, &tm1.tm_mon, &tm1.tm_mday);
    sscanf(date2, "%d-%d-%d", &tm2.tm_year, &tm2.tm_mon, &tm2.tm_mday);
    
    tm1.tm_year -= 1900;
    tm1.tm_mon -= 1;
    tm2.tm_year -= 1900;
    tm2.tm_mon -= 1;
    
    time_t time1 = mktime(&tm1);
    time_t time2 = mktime(&tm2);
    
    return (int)(difftime(time2, time1) / (60 * 60 * 24));
}

// 检查待处理提醒的回调函数
static int check_pending_callback(void* data, int argc, char** argv, char** col_names) {
    PendingReminder pending;
    memset(&pending, 0, sizeof(PendingReminder));
    
    for (int i = 0; i < argc; i++) {
        if (argv[i] == NULL) continue;
        
        if (strcmp(col_names[i], "plant_id") == 0) {
            pending.plant_id = atoi(argv[i]);
        } else if (strcmp(col_names[i], "plant_name") == 0) {
            dao_strncpy_safe(pending.plant_name, argv[i], sizeof(pending.plant_name));
        } else if (strcmp(col_names[i], "reminder_type") == 0) {
            pending.reminder_type = string_to_reminder_type(argv[i]);
        } else if (strcmp(col_names[i], "frequency") == 0) {
            // 频率信息在查询中已经使用
        } else if (strcmp(col_names[i], "last_operation_date") == 0) {
            dao_strncpy_safe(pending.last_operation_date, argv[i], sizeof(pending.last_operation_date));
        } else if (strcmp(col_names[i], "days_since_last") == 0) {
            pending.days_since_last = atoi(argv[i]);
        }
    }
    
    // 调用用户回调函数
    void (*callback)(const PendingReminder*) = data;
    if (callback) {
        callback(&pending);
    }
    
    return 0;
}

// 检查待处理提醒
DAO_RESULT reminders_dao_check_pending(void (*callback)(const PendingReminder* reminder)) {
    char *err_msg = NULL;
    
    // 复杂的SQL查询，检查需要提醒的记录
    const char* sql = 
        "SELECT r.plant_id, p.name as plant_name, r.reminder_type, "
        "COALESCE(cr.operation_date, p.planting_date) as last_operation_date, "
        "julianday('now') - julianday(COALESCE(cr.operation_date, p.planting_date)) as days_since_last "
        "FROM reminders r "
        "JOIN plants p ON r.plant_id = p.id "
        "LEFT JOIN care_records cr ON (r.plant_id = cr.plant_id AND "
        "    CASE r.reminder_type WHEN '浇水' THEN cr.operation_type = '浇水' "
        "                        WHEN '施肥' THEN cr.operation_type = '施肥' END "
        "    AND cr.operation_date = (SELECT MAX(operation_date) FROM care_records "
        "                           WHERE plant_id = r.plant_id AND "
        "                           CASE r.reminder_type WHEN '浇水' THEN operation_type = '浇水' "
        "                                           WHEN '施肥' THEN operation_type = '施肥' END)) "
        "WHERE r.is_active = 1 "
        "AND (julianday('now') - julianday(COALESCE(cr.operation_date, p.planting_date))) >= r.frequency";
    
    int rc = sqlite3_exec(db, sql, check_pending_callback, (void*)callback, &err_msg);
    
    if (rc != SQLITE_OK) {
        DAO_CHECK_SQL(rc, err_msg);
        return DAO_DB_ERROR;
    }
    
    return DAO_SUCCESS;
}