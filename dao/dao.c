#include "dao.h"
#include "../database.h"

// DAO层初始化
int dao_init() {
    // 初始化数据库连接
    if (init_database() != 0) {
        return DAO_ERROR;
    }
    
    // 创建表结构
    if (create_tables() != 0) {
        close_database();
        return DAO_ERROR;
    }
    
    return DAO_SUCCESS;
}

// DAO层清理
void dao_cleanup() {
    close_database();
}