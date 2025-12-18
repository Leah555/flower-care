#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include "database.h"

int main() {
    printf("测试数据库初始化...\n");
    
    // 检查当前工作目录
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("当前工作目录: %s\n", cwd);
    }
    
    // 检查数据库文件是否存在
    struct stat st;
    if (stat(DATABASE_PATH, &st) == 0) {
        printf("数据库文件存在，大小: %ld bytes\n", st.st_size);
        printf("文件权限: %o\n", st.st_mode & 0777);
    } else {
        printf("数据库文件不存在，将创建新文件\n");
    }
    
    // 测试数据库初始化
    printf("初始化数据库...\n");
    int result = init_database();
    if (result == 0) {
        printf("数据库初始化成功\n");
        
        // 测试创建表
        printf("创建表结构...\n");
        result = create_tables();
        if (result == 0) {
            printf("表创建成功\n");
        } else {
            printf("表创建失败\n");
        }
        
        close_database();
    } else {
        printf("数据库初始化失败\n");
    }
    
    return 0;
}