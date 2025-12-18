#!/bin/bash

# macOS 版本的编译脚本
# 注意：此脚本使用 stub 实现替代 Linux/ARM 特定的库

echo "开始编译（macOS版本）..."

gcc \
    main.c \
    interface.c \
    database.c \
    event.c \
    lcd.c \
    sqlite3.c \
    font_1_stub.c \
    dao/dao.c \
    dao/dao_common.c \
    dao/plants_dao.c \
    dao/care_records_dao.c \
    dao/reminders_dao.c \
    dao/growth_records_dao.c \
    dao/care_experience_dao.c \
    -o main \
    -I ./ \
    -I ./dao \
    -lm \
    -lpthread \
    -ldl

if [ $? -eq 0 ]; then
    echo "编译成功！"
    echo "生成的可执行文件: main"
    echo ""
    echo "注意：此版本使用 stub 实现，LCD 和触摸屏功能可能无法正常工作"
    echo "此版本主要用于测试数据库和界面逻辑"
else
    echo "编译失败！"
    exit 1
fi

