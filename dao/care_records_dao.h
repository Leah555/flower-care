#ifndef CARE_RECORDS_DAO_H
#define CARE_RECORDS_DAO_H

#include "dao_common.h"

// 养护记录类型枚举
typedef enum {
    CARE_WATER = 0,     // 浇水
    CARE_FERTILIZE = 1, // 施肥
    CARE_PEST_CONTROL = 2, // 病虫害防治
    CARE_OTHER = 3      // 其他
} CareType;

// 养护记录结构体
typedef struct {
    int id;                     // 记录ID
    int plant_id;               // 花卉ID
    
    // 操作类型细化
    char operation_category[21]; // 操作类别
    CareType operation_type;    // 操作类型
    
    char operation_date[20];    // 操作时间 YYYY-MM-DD HH:MM:SS
    
    // 操作详情
    char details[256];          // 操作详情
    char amount[21];            // 用量
    int duration_minutes;       // 操作时长（分钟）
    
    // 效果记录
    char plant_condition_before[21]; // 操作前植物状态
    char plant_condition_after[21];  // 操作后植物状态
    int effectiveness_rating;   // 效果评分（1-5）
    
    // 病虫害相关（如果是病虫害操作）
    char pest_type[31];         // 病虫害类型
    char control_method[51];    // 防治方法
    char pesticide_used[51];    // 使用农药
    
    // 环境条件
    int temperature;            // 温度
    int humidity;               // 湿度
    char weather_condition[21]; // 天气状况
    
    char notes[256];            // 备注
} CareRecord;

// 养护记录查询条件结构体
typedef struct {
    int plant_id;               // 花卉ID
    CareType operation_type;    // 操作类型
    char start_date[11];        // 开始日期 YYYY-MM-DD
    char end_date[11];          // 结束日期 YYYY-MM-DD
} CareRecordQuery;

// 初始化CareRecord结构体
void care_record_init(CareRecord* record);

// 添加养护记录
DAO_RESULT care_records_dao_add(const CareRecord* record);

// 根据ID删除养护记录
DAO_RESULT care_records_dao_delete(int record_id);

// 根据ID查询养护记录
DAO_RESULT care_records_dao_get_by_id(int record_id, CareRecord* record);

// 查询花卉的所有养护记录
DAO_RESULT care_records_dao_get_by_plant(int plant_id, void (*callback)(const CareRecord* record));

// 根据条件查询养护记录
DAO_RESULT care_records_dao_query(const CareRecordQuery* query, void (*callback)(const CareRecord* record));

// 统计某花卉的养护次数
DAO_RESULT care_records_dao_count_by_plant(int plant_id, CareType type, int* count);

// 获取最近一次养护记录
DAO_RESULT care_records_dao_get_latest(int plant_id, CareType type, CareRecord* record);

// 将操作类型枚举转换为字符串
const char* care_type_to_string(CareType type);

// 将字符串转换为操作类型枚举
CareType string_to_care_type(const char* type_str);

#endif