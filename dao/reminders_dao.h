#ifndef REMINDERS_DAO_H
#define REMINDERS_DAO_H

#include "dao_common.h"

// 提醒类型枚举
typedef enum {
    REMINDER_WATER = 0,     // 浇水提醒
    REMINDER_FERTILIZE = 1  // 施肥提醒
} ReminderType;

// 提醒设置结构体
typedef struct {
    int id;                     // 提醒ID
    int plant_id;               // 花卉ID
    ReminderType reminder_type; // 提醒类型
    int frequency;              // 频率（天）
    char last_reminder_date[11]; // 最后提醒日期 YYYY-MM-DD
    int is_active;              // 是否激活（1激活，0不激活）
} Reminder;

// 提醒查询条件结构体
typedef struct {
    int plant_id;               // 花卉ID
    ReminderType reminder_type; // 提醒类型
    int is_active;              // 是否激活
} ReminderQuery;

// 待处理提醒信息结构体
typedef struct {
    int plant_id;               // 花卉ID
    char plant_name[51];        // 花卉名称
    ReminderType reminder_type; // 提醒类型
    char last_operation_date[11]; // 最后操作日期
    int days_since_last;        // 距离上次操作天数
} PendingReminder;

// 初始化Reminder结构体
void reminder_init(Reminder* reminder);

// 添加提醒设置
DAO_RESULT reminders_dao_add(const Reminder* reminder);

// 根据ID更新提醒设置
DAO_RESULT reminders_dao_update(int reminder_id, const Reminder* reminder);

// 根据ID删除提醒设置
DAO_RESULT reminders_dao_delete(int reminder_id);

// 根据ID查询提醒设置
DAO_RESULT reminders_dao_get_by_id(int reminder_id, Reminder* reminder);

// 查询花卉的所有提醒设置
DAO_RESULT reminders_dao_get_by_plant(int plant_id, void (*callback)(const Reminder* reminder));

// 查询所有提醒设置
DAO_RESULT reminders_dao_get_all(void (*callback)(const Reminder* reminder));

// 根据条件查询提醒设置
DAO_RESULT reminders_dao_query(const ReminderQuery* query, void (*callback)(const Reminder* reminder));

// 更新最后提醒日期
DAO_RESULT reminders_dao_update_last_reminder(int reminder_id, const char* date);

// 激活/停用提醒
DAO_RESULT reminders_dao_set_active(int reminder_id, int is_active);

// 检查待处理提醒
DAO_RESULT reminders_dao_check_pending(void (*callback)(const PendingReminder* reminder));

// 将提醒类型枚举转换为字符串
const char* reminder_type_to_string(ReminderType type);

// 将字符串转换为提醒类型枚举
ReminderType string_to_reminder_type(const char* type_str);

#endif