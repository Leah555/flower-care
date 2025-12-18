#ifndef DAO_H
#define DAO_H

// 包含所有DAO头文件
#include "dao_common.h"
#include "plants_dao.h"
#include "care_records_dao.h"
#include "reminders_dao.h"

// DAO层初始化
int dao_init();

// DAO层清理
void dao_cleanup();

#endif