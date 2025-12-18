#ifndef CARE_EXPERIENCE_DAO_H
#define CARE_EXPERIENCE_DAO_H

#include <sqlite3.h>
#include "../database.h"
#include "dao_common.h"

// 养护经验结构体
typedef struct {
    int id;                         // 经验ID
    char variety[51];              // 花卉品种
    
    // 成功经验
    int optimal_water_frequency;    // 最佳浇水频率（天）
    int optimal_fertilize_frequency; // 最佳施肥频率（天）
    char best_season[21];          // 最佳生长季节
    char common_pests[256];        // 常见病虫害
    char effective_controls[256];  // 有效防治方法
    
    // 失败教训
    char common_mistakes[256];     // 常见错误
    char warning_signs[256];       // 预警信号
    char recovery_methods[256];    // 恢复方法
    
    // 统计分析
    int total_plants;              // 总养护数量
    double success_rate;           // 成功率（%）
    double avg_health_score;       // 平均健康评分
    
    char last_updated[11];         // 最后更新日期
    int confidence_level;          // 置信度等级（1-5）
} CareExperience;

// 养护经验查询结构体
typedef struct {
    char variety[51];              // 品种查询条件
    char best_season[21];          // 季节查询条件
    int min_confidence_level;      // 最小置信度
    int max_confidence_level;      // 最大置信度
} CareExperienceQuery;

// 初始化养护经验结构体
void care_experience_init(CareExperience* experience);

// DAO操作函数声明
DAO_RESULT care_experience_dao_add(CareExperience* experience);
DAO_RESULT care_experience_dao_update(int id, CareExperience* experience);
DAO_RESULT care_experience_dao_delete(int id);
DAO_RESULT care_experience_dao_get_by_id(int id, CareExperience* experience);
DAO_RESULT care_experience_dao_get_all(void (*callback)(const CareExperience*));
DAO_RESULT care_experience_dao_query(CareExperienceQuery* query, void (*callback)(const CareExperience*));

#endif