#include <stdio.h>
#include <stdlib.h>
#include "database.h"

int main() {
    printf("=== 数据库手动创建测试程序 ===\n");
    
    // 测试1：手动创建数据库
    printf("\n1. 测试手动创建数据库...\n");
    if (create_database_manually() == 0) {
        printf("✓ 手动创建数据库成功\n");
    } else {
        printf("✗ 手动创建数据库失败\n");
        return -1;
    }
    
    // 测试2：初始化数据库连接
    printf("\n2. 测试数据库初始化...\n");
    if (init_database() == 0) {
        printf("✓ 数据库初始化成功\n");
    } else {
        printf("✗ 数据库初始化失败\n");
        return -1;
    }
    
    // 测试3：检查表是否存在
    printf("\n3. 测试表结构检查...\n");
    if (check_tables_exist() == 0) {
        printf("✓ 表结构检查通过\n");
    } else {
        printf("✗ 表结构检查失败\n");
        return -1;
    }
    
    // 测试4：重置数据库
    printf("\n4. 测试数据库重置...\n");
    if (reset_database() == 0) {
        printf("✓ 数据库重置成功\n");
    } else {
        printf("✗ 数据库重置失败\n");
        return -1;
    }
    
    // 测试5：再次初始化验证
    printf("\n5. 测试重置后的数据库初始化...\n");
    if (init_database() == 0) {
        printf("✓ 重置后数据库初始化成功\n");
    } else {
        printf("✗ 重置后数据库初始化失败\n");
        return -1;
    }
    
    // 关闭数据库
    close_database();
    
    printf("\n=== 所有测试通过！数据库健壮性得到保障 ===\n");
    return 0;
}