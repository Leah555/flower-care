#!/bin/bash

echo "修复数据库初始化问题..."

# 检查当前目录
pwd
ls -la

# 检查数据库文件权限
if [ -f "flower_care.db" ]; then
    echo "数据库文件存在，检查权限:"
    ls -la flower_care.db
    
    # 修复文件权限
    chmod 644 flower_care.db
    echo "修复文件权限完成"
else
    echo "数据库文件不存在，将创建新文件"
fi

# 检查SQLite3库
if command -v sqlite3 &> /dev/null; then
    echo "SQLite3已安装，版本:"
    sqlite3 --version
else
    echo "警告: SQLite3未安装"
fi

# 尝试使用绝对路径编译和测试
echo "使用绝对路径测试数据库..."

# 获取当前绝对路径
CURRENT_DIR=$(pwd)
DB_PATH="$CURRENT_DIR/flower_care.db"

echo "数据库绝对路径: $DB_PATH"

# 修改database.h中的数据库路径
sed -i "s|#define DATABASE_PATH "./flower_care.db"|#define DATABASE_PATH \"$DB_PATH\"|" database.h

echo "数据库路径已更新为绝对路径"

# 编译测试程序
echo "编译测试程序..."
arm-linux-gcc test_database.c database.c sqlite3.c -o test_database -I ./ -lm -lpthread -ldl

if [ $? -eq 0 ]; then
    echo "编译成功，运行测试程序..."
    ./test_database
else
    echo "编译失败"
fi