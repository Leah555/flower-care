#!/bin/bash

# 编译所有.c文件的完整命令（Linux环境）
arm-linux-gcc \
    main.c \
    interface.c \
    database.c \
    event.c \
    lcd.c \
    sqlite3.c \
    dao/dao.c \
    dao/dao_common.c \
    dao/plants_dao.c \
    dao/care_records_dao.c \
    dao/reminders_dao.c \
    -o main \
    -I ./ \
    -I ./dao \
    -L ./ \
    -lfont_1 \
    -lm \
    -lpthread \
    -ldl

if [ $? -eq 0 ]; then
    echo "编译成功！"
    echo "生成的可执行文件: main"
else
    echo "编译失败！"
    exit 1
fi

echo ""
echo "简化版本命令（如果上述命令失败）："
echo "arm-linux-gcc event.c lcd.c main.c sqlite3.c -o main -I ./ -L ./ -lfont_1 -lm -lpthread -ldl"

echo ""
echo "Windows版本命令："
echo "gcc main.c interface.c database.c event.c lcd.c sqlite3.c dao/dao.c dao/dao_common.c dao/plants_dao.c dao/care_records_dao.c dao/reminders_dao.c -o main.exe -I ./ -I ./dao -L ./ -lfont_1 -lm -lpthread -ldl"