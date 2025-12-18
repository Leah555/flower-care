#ifndef PLANTS_DAO_H
#define PLANTS_DAO_H

#include "dao_common.h"

// 花卉信息结构体
typedef struct {
    int id;                     // 花卉ID
    char name[51];             // 花卉名称
    char variety[51];          // 品种
    char planting_date[11];    // 种植日期 YYYY-MM-DD
    int water_frequency;       // 浇水频率（天）
    char last_water_date[11];  // 最后浇水日期
    char last_fertilize_date[11]; // 最后施肥日期
    char status[21];           // 生长状态
    char notes[256];           // 备注信息
} Plant;

// 花卉查询条件结构体
typedef struct {
    char name[51];             // 花卉名称（模糊匹配）
    char variety[51];          // 品种（模糊匹配）
    char status[21];           // 生长状态
} PlantQuery;

// 初始化Plant结构体
void plant_init(Plant* plant);

// 添加花卉信息
DAO_RESULT plants_dao_add(const Plant* plant);

// 根据ID更新花卉信息
DAO_RESULT plants_dao_update(int plant_id, const Plant* plant);

// 根据ID删除花卉信息
DAO_RESULT plants_dao_delete(int plant_id);

// 根据ID查询花卉信息
DAO_RESULT plants_dao_get_by_id(int plant_id, Plant* plant);

// 查询所有花卉信息
DAO_RESULT plants_dao_get_all(void (*callback)(const Plant* plant));

// 根据条件查询花卉信息
DAO_RESULT plants_dao_query(const PlantQuery* query, void (*callback)(const Plant* plant));

// 更新最后浇水日期
DAO_RESULT plants_dao_update_last_water(int plant_id, const char* date);

// 更新最后施肥日期
DAO_RESULT plants_dao_update_last_fertilize(int plant_id, const char* date);

// 更新生长状态
DAO_RESULT plants_dao_update_status(int plant_id, const char* status);

#endif