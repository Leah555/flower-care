#ifndef GROWTH_RECORDS_DAO_H
#define GROWTH_RECORDS_DAO_H

#include "dao_common.h"

// 生长记录结构体
typedef struct {
    int id;                     // 记录ID
    int plant_id;               // 花卉ID
    char record_date[11];       // 记录日期 YYYY-MM-DD
    
    // 生长指标
    double height_cm;           // 高度（厘米）
    int leaf_count;             // 叶片数量
    int bud_count;              // 花蕾数量
    int flower_count;           // 花朵数量
    
    // 健康状况
    int health_score;           // 健康评分（0-100）
    char leaf_color[21];        // 叶片颜色
    char growth_vigor[21];      // 生长活力
    
    // 环境因素
    int temperature;            // 温度
    int humidity;               // 湿度
    char light_exposure[21];    // 光照情况
    
    char notes[256];            // 备注
    char photo_path[201];       // 照片路径
} GrowthRecord;

// 生长记录查询条件结构体
typedef struct {
    int plant_id;               // 花卉ID
    char start_date[11];        // 开始日期 YYYY-MM-DD
    char end_date[11];          // 结束日期 YYYY-MM-DD
} GrowthRecordQuery;

// 初始化GrowthRecord结构体
void growth_record_init(GrowthRecord* record);

// 添加生长记录
DAO_RESULT growth_records_dao_add(const GrowthRecord* record);

// 根据ID更新生长记录
DAO_RESULT growth_records_dao_update(int record_id, const GrowthRecord* record);

// 根据ID删除生长记录
DAO_RESULT growth_records_dao_delete(int record_id);

// 根据ID查询生长记录
DAO_RESULT growth_records_dao_get_by_id(int record_id, GrowthRecord* record);

// 查询花卉的所有生长记录
DAO_RESULT growth_records_dao_get_by_plant(int plant_id, void (*callback)(const GrowthRecord* record));

// 根据条件查询生长记录
DAO_RESULT growth_records_dao_query(const GrowthRecordQuery* query, void (*callback)(const GrowthRecord* record));

#endif