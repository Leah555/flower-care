#include "interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "font_1.h"

// 全局接口实例指针定义
Interface* interface_instance = NULL;

// 植物数据收集回调函数
static void plants_collect_callback(const Plant* plant) {
    if (!interface_instance || !plant) return;
    
    Interface *interface = interface_instance;
    
    if (interface->plant_count >= interface->plants_capacity) {
        interface->plants_capacity += 10; // 每次增加10个容量
        Plant *temp = realloc(interface->plants, interface->plants_capacity * sizeof(Plant));
        if (!temp) return; // 内存分配失败
        interface->plants = temp;
    }
    
    // 复制植物数据
    memcpy(&interface->plants[interface->plant_count], plant, sizeof(Plant));
    interface->plant_count++;
}

// 养护记录数据收集回调函数
static void care_records_collect_callback(const CareRecord* record) {
    if (!interface_instance || !record) return;
    
    Interface *interface = interface_instance;
    
    if (interface->care_record_count >= interface->care_records_capacity) {
        interface->care_records_capacity += 10; // 每次增加10个容量
        CareRecord *temp = realloc(interface->care_records, interface->care_records_capacity * sizeof(CareRecord));
        if (!temp) return; // 内存分配失败
        interface->care_records = temp;
    }
    
    // 复制养护记录数据
    memcpy(&interface->care_records[interface->care_record_count], record, sizeof(CareRecord));
    interface->care_record_count++;
}

// 提醒数据收集回调函数
static void reminders_collect_callback(const Reminder* reminder) {
    if (!interface_instance || !reminder) return;
    
    Interface *interface = interface_instance;
    
    if (interface->reminder_count >= interface->reminders_capacity) {
        interface->reminders_capacity += 10; // 每次增加10个容量
        Reminder *temp = realloc(interface->reminders, interface->reminders_capacity * sizeof(Reminder));
        if (!temp) return; // 内存分配失败
        interface->reminders = temp;
    }
    
    // 复制提醒数据
    memcpy(&interface->reminders[interface->reminder_count], reminder, sizeof(Reminder));
    interface->reminder_count++;
}

#define BACKGROUND_COLOR 0xFFFFFF
#define HEADER_COLOR 0x4CAF50
#define FOOTER_COLOR 0x2196F3
#define BUTTON_COLOR 0x607D8B
#define BUTTON_HOVER_COLOR 0x455A64
#define TEXT_COLOR 0x000000
#define HIGHLIGHT_COLOR 0xFF9800

Interface* interface_init() {
    Interface *interface = malloc(sizeof(Interface));
    if (!interface) {
        return NULL;
    }
    
    // 初始化DAO层
    if (dao_init() != 0) {
        free(interface);
        return NULL;
    }
    
    interface->lcd = LCD_init();
    if (interface->lcd.fd == -1) {
        dao_cleanup();
        free(interface);
        return NULL;
    }
    
    interface->current_screen = MAIN_MENU;
    interface->plants = NULL;
    interface->plant_count = 0;
    interface->plants_capacity = 0;
    interface->care_records = NULL;
    interface->care_record_count = 0;
    interface->care_records_capacity = 0;
    interface->reminders = NULL;
    interface->reminder_count = 0;
    interface->reminders_capacity = 0;
    interface->selected_plant_id = -1;
    interface->selected_care_record_id = -1;
    interface->selected_reminder_id = -1;
    
    return interface;
}

void interface_cleanup(Interface *interface) {
    if (!interface) return;
    
    if (interface->plants) {
        free(interface->plants);
    }
    if (interface->care_records) {
        free(interface->care_records);
    }
    if (interface->reminders) {
        free(interface->reminders);
    }
    
    // 清理DAO层
    dao_cleanup();
    
    // Linux环境下使用munmap释放内存映射
    #ifdef __linux__
    munmap(interface->lcd.plcd, SCREEN_WIDTH * SCREEN_HEIGHT * 4);
    #endif
    close(interface->lcd.fd);
    free(interface);
}

void interface_draw_button(Interface *interface, Button *button) {
    int x = button->pos_x;
    int y = button->pos_y;
    int width = button->btn_width;
    int height = button->btn_height;
    int color = button->btn_color;
    int text_color = button->btn_text_color;
    char *text = button->btn_text;
    
    LCD_show_rec(x, y, width, height, color, interface->lcd.plcd);
    
    int text_x = x + (width - strlen(text) * 8) / 2;
    int text_y = y + (height - 16) / 2;
    
    showString(interface->lcd.plcd, text, 16, text_color, text_x, text_y, color);
}

int interface_is_point_in_button(struct point p, Button button) {
    return (p.x >= button.pos_x && p.x <= button.pos_x + button.btn_width &&
            p.y >= button.pos_y && p.y <= button.pos_y + button.btn_height);
}

void interface_draw_header(Interface *interface, const char *title) {
    LCD_show_rec(0, 0, SCREEN_WIDTH, HEADER_HEIGHT, HEADER_COLOR, interface->lcd.plcd);
    
    int title_x = (SCREEN_WIDTH - strlen(title) * 8) / 2;
    int title_y = (HEADER_HEIGHT - 16) / 2;
    
    showString(interface->lcd.plcd, title, 16, TEXT_COLOR, title_x, title_y, HEADER_COLOR);
}

void interface_draw_footer(Interface *interface) {
    LCD_show_rec(0, SCREEN_HEIGHT - FOOTER_HEIGHT, SCREEN_WIDTH, FOOTER_HEIGHT, FOOTER_COLOR, interface->lcd.plcd);
    
    const char *back_text = "返回";
    int back_x = 20;
    int back_y = SCREEN_HEIGHT - FOOTER_HEIGHT + (FOOTER_HEIGHT - 16) / 2;
    
    showString(interface->lcd.plcd, back_text, 16, TEXT_COLOR, back_x, back_y, FOOTER_COLOR);
}

void interface_draw_main_menu(Interface *interface) {
    LCD_show_rec(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BACKGROUND_COLOR, interface->lcd.plcd);
    interface_draw_header(interface, "花卉养护记录系统");
    
    Button buttons[] = {
        {(SCREEN_WIDTH - BUTTON_WIDTH) / 2, HEADER_HEIGHT + BUTTON_MARGIN, BUTTON_WIDTH, BUTTON_HEIGHT, "植物管理", BUTTON_COLOR, TEXT_COLOR},
        {(SCREEN_WIDTH - BUTTON_WIDTH) / 2, HEADER_HEIGHT + BUTTON_MARGIN * 2 + BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT, "养护记录", BUTTON_COLOR, TEXT_COLOR},
        {(SCREEN_WIDTH - BUTTON_WIDTH) / 2, HEADER_HEIGHT + BUTTON_MARGIN * 3 + BUTTON_HEIGHT * 2, BUTTON_WIDTH, BUTTON_HEIGHT, "提醒管理", BUTTON_COLOR, TEXT_COLOR},
        {(SCREEN_WIDTH - BUTTON_WIDTH) / 2, HEADER_HEIGHT + BUTTON_MARGIN * 4 + BUTTON_HEIGHT * 3, BUTTON_WIDTH, BUTTON_HEIGHT, "系统设置", BUTTON_COLOR, TEXT_COLOR}
    };
    
    for (int i = 0; i < 4; i++) {
        interface_draw_button(interface, &buttons[i]);
    }
    
    interface_draw_footer(interface);
}

void interface_draw_plant_list(Interface *interface) {
    LCD_show_rec(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BACKGROUND_COLOR, interface->lcd.plcd);
    interface_draw_header(interface, "植物列表");
    
    Button add_button = {SCREEN_WIDTH - BUTTON_WIDTH - 20, HEADER_HEIGHT + 10, BUTTON_WIDTH, BUTTON_HEIGHT, "添加植物", BUTTON_COLOR, TEXT_COLOR};
    interface_draw_button(interface, &add_button);
    
    int start_y = HEADER_HEIGHT + BUTTON_HEIGHT + 30;
    int item_height = 50;
    
    if (interface->plant_count == 0) {
        const char *no_plants = "暂无植物记录";
        int text_x = (SCREEN_WIDTH - strlen(no_plants) * 8) / 2;
        int text_y = start_y + 50;
        
        showString(interface->lcd.plcd, no_plants, 16, TEXT_COLOR, text_x, text_y, BACKGROUND_COLOR);
    } else {
        for (int i = 0; i < interface->plant_count && i < 6; i++) {
            int y = start_y + i * item_height;
            
            char plant_info[100];
            snprintf(plant_info, sizeof(plant_info), "%s - %s", interface->plants[i].name, interface->plants[i].variety);
            
            LCD_show_rec(20, y, SCREEN_WIDTH - 40, item_height - 5, 0xF5F5F5, interface->lcd.plcd);
            
            showString(interface->lcd.plcd, plant_info, 16, TEXT_COLOR, 30, y + 15, 0xF5F5F5);
        }
    }
    
    interface_draw_footer(interface);
}

void interface_draw_plant_detail(Interface *interface, int plant_id) {
    Plant *plant = NULL;
    for (int i = 0; i < interface->plant_count; i++) {
        if (interface->plants[i].id == plant_id) {
            plant = &interface->plants[i];
            break;
        }
    }
    
    if (!plant) return;
    
    LCD_show_rec(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BACKGROUND_COLOR, interface->lcd.plcd);
    interface_draw_header(interface, "植物详情");
    
    int y = HEADER_HEIGHT + 20;
    
    char info[100];
    snprintf(info, sizeof(info), "名称: %s", plant->name);
    showString(interface->lcd.plcd, info, 16, TEXT_COLOR, 30, y, BACKGROUND_COLOR);
    
    y += 30;
    snprintf(info, sizeof(info), "品种: %s", plant->variety);
    showString(interface->lcd.plcd, info, 16, TEXT_COLOR, 30, y, BACKGROUND_COLOR);
    
    y += 30;
    snprintf(info, sizeof(info), "浇水频率: %d天", plant->water_frequency);
    showString(interface->lcd.plcd, info, 16, TEXT_COLOR, 30, y, BACKGROUND_COLOR);
    
    y += 30;
    snprintf(info, sizeof(info), "最后浇水: %s", plant->last_water_date);
    showString(interface->lcd.plcd, info, 16, TEXT_COLOR, 30, y, BACKGROUND_COLOR);
    
    y += 30;
    snprintf(info, sizeof(info), "最后施肥: %s", plant->last_fertilize_date);
    showString(interface->lcd.plcd, info, 16, TEXT_COLOR, 30, y, BACKGROUND_COLOR);
    
    y += 30;
    snprintf(info, sizeof(info), "状态: %s", plant->status);
    showString(interface->lcd.plcd, info, 16, TEXT_COLOR, 30, y, BACKGROUND_COLOR);
    
    Button care_button = {30, y + 50, BUTTON_WIDTH, BUTTON_HEIGHT, "查看养护记录", BUTTON_COLOR, TEXT_COLOR};
    interface_draw_button(interface, &care_button);
    
    Button edit_button = {30 + BUTTON_WIDTH + 20, y + 50, BUTTON_WIDTH, BUTTON_HEIGHT, "编辑植物", BUTTON_COLOR, TEXT_COLOR};
    interface_draw_button(interface, &edit_button);
    
    interface_draw_footer(interface);
}

void interface_handle_touch(Interface *interface, struct point touch_point) {
    if (touch_point.x == -1 || touch_point.y == -1) return;
    
    switch (interface->current_screen) {
        case MAIN_MENU:
            if (touch_point.y >= SCREEN_HEIGHT - FOOTER_HEIGHT) {
                return;
            }
            
            int button_y_start = HEADER_HEIGHT + BUTTON_MARGIN;
            for (int i = 0; i < 4; i++) {
                Button test_button = {(SCREEN_WIDTH - BUTTON_WIDTH) / 2, button_y_start + i * (BUTTON_HEIGHT + BUTTON_MARGIN), BUTTON_WIDTH, BUTTON_HEIGHT, "", 0, 0};
                
                if (interface_is_point_in_button(touch_point, test_button)) {
                    switch (i) {
                        case 0: interface_navigate_to(interface, PLANT_LIST); break;
                        case 1: interface_navigate_to(interface, CARE_RECORDS); break;
                        case 2: interface_navigate_to(interface, REMINDERS); break;
                        case 3: interface_navigate_to(interface, SETTINGS); break;
                    }
                    return;
                }
            }
            break;
            
        case REMINDERS:
            if (touch_point.y >= SCREEN_HEIGHT - FOOTER_HEIGHT) {
                interface_navigate_to(interface, MAIN_MENU);
                return;
            }
            
            // Check if "Add Reminder" button was pressed
            Button add_reminder_btn = {SCREEN_WIDTH - BUTTON_WIDTH - 20, HEADER_HEIGHT + 10, BUTTON_WIDTH, BUTTON_HEIGHT, "", 0, 0};
            if (interface_is_point_in_button(touch_point, add_reminder_btn)) {
                // TODO: Navigate to Add Reminder screen when implemented
                return;
            }
            
            // Check if any reminder item was pressed
            int reminder_start_y = HEADER_HEIGHT + BUTTON_HEIGHT + 30;
            int reminder_item_height = 70;
            
            for (int i = 0; i < interface->reminder_count && i < 5; i++) {
                int y = reminder_start_y + i * reminder_item_height;
                Button reminder_item_btn = {20, y, SCREEN_WIDTH - 40, reminder_item_height - 5, "", 0, 0};
                
                if (interface_is_point_in_button(touch_point, reminder_item_btn)) {
                    interface->selected_reminder_id = interface->reminders[i].id;
                    // TODO: Navigate to Reminder Detail/Edit screen when implemented
                    return;
                }
            }
            break;
            
        case PLANT_LIST:
            if (touch_point.y >= SCREEN_HEIGHT - FOOTER_HEIGHT) {
                interface_navigate_to(interface, MAIN_MENU);
                return;
            }
            
            Button add_button = {SCREEN_WIDTH - BUTTON_WIDTH - 20, HEADER_HEIGHT + 10, BUTTON_WIDTH, BUTTON_HEIGHT, "", 0, 0};
            if (interface_is_point_in_button(touch_point, add_button)) {
                interface_navigate_to(interface, ADD_PLANT);
                return;
            }
            
            int start_y = HEADER_HEIGHT + BUTTON_HEIGHT + 30;
            int item_height = 50;
            
            for (int i = 0; i < interface->plant_count && i < 6; i++) {
                int y = start_y + i * item_height;
                Button plant_button = {20, y, SCREEN_WIDTH - 40, item_height - 5, "", 0, 0};
                
                if (interface_is_point_in_button(touch_point, plant_button)) {
                    interface->selected_plant_id = interface->plants[i].id;
                    interface_navigate_to(interface, PLANT_DETAIL);
                    return;
                }
            }
            break;
            
        case PLANT_DETAIL:
            if (touch_point.y >= SCREEN_HEIGHT - FOOTER_HEIGHT) {
                interface_navigate_to(interface, PLANT_LIST);
                return;
            }
            
            // 计算"查看养护记录"和"编辑植物"按钮的位置
            int detail_y = HEADER_HEIGHT + 20;
            for (int i = 0; i < 6; i++) { // 跳过6行信息显示
                detail_y += 30;
            }
            
            Button care_button = {30, detail_y + 50, BUTTON_WIDTH, BUTTON_HEIGHT, "", 0, 0};
            Button edit_button = {30 + BUTTON_WIDTH + 20, detail_y + 50, BUTTON_WIDTH, BUTTON_HEIGHT, "", 0, 0};
            
            if (interface_is_point_in_button(touch_point, care_button)) {
                interface_navigate_to(interface, CARE_RECORDS);
                return;
            }
            
            if (interface_is_point_in_button(touch_point, edit_button)) {
                // 编辑植物功能暂未实现
                return;
            }
            break;
            
        default:
            if (touch_point.y >= SCREEN_HEIGHT - FOOTER_HEIGHT) {
                interface_navigate_to(interface, MAIN_MENU);
            }
            break;
    }
}

// 获取植物数据的真实实现
static int interface_get_plants(Interface *interface) {
    // 重置计数器
    interface->plant_count = 0;
    
    // 确保有足够的容量
    if (interface->plants_capacity < 10) {
        interface->plants_capacity = 10;
        interface->plants = realloc(interface->plants, interface->plants_capacity * sizeof(Plant));
    }
    
    // 保存接口实例以便在回调函数中使用
    interface_instance = interface;
    
    // 调用DAO层获取所有植物数据
    DAO_RESULT result = plants_dao_get_all(plants_collect_callback);
    
    if (result != DAO_SUCCESS) {
        printf("获取植物数据失败\n");
        return -1;
    }
    
    return interface->plant_count;
}

// 获取特定植物的养护记录
static int interface_get_care_records(Interface *interface, int plant_id) {
    // 重置计数器
    interface->care_record_count = 0;
    
    // 确保有足够的容量
    if (interface->care_records_capacity < 10) {
        interface->care_records_capacity = 10;
        interface->care_records = realloc(interface->care_records, interface->care_records_capacity * sizeof(CareRecord));
    }
    
    // 保存接口实例以便在回调函数中使用
    interface_instance = interface;
    
    // 调用DAO层获取指定植物的所有养护记录
    DAO_RESULT result = care_records_dao_get_by_plant(plant_id, care_records_collect_callback);
    
    if (result != DAO_SUCCESS) {
        printf("获取养护记录失败\n");
        return -1;
    }
    
    return interface->care_record_count;
}

// 获取提醒数据
static int interface_get_reminders(Interface *interface) {
    // 重置计数器
    interface->reminder_count = 0;
    
    // 确保有足够的容量
    if (interface->reminders_capacity < 10) {
        interface->reminders_capacity = 10;
        interface->reminders = realloc(interface->reminders, interface->reminders_capacity * sizeof(Reminder));
    }
    
    // 保存接口实例以便在回调函数中使用
    interface_instance = interface;
    
    // 调用DAO层获取所有提醒数据
    DAO_RESULT result = reminders_dao_get_all(reminders_collect_callback);
    
    if (result != DAO_SUCCESS) {
        printf("获取提醒数据失败\n");
        return -1;
    }
    
    return interface->reminder_count;
}

void interface_draw_care_records(Interface *interface, int plant_id) {
    // 获取植物信息用于标题显示
    Plant plant_info;
    plant_init(&plant_info);
    DAO_RESULT result = plants_dao_get_by_id(plant_id, &plant_info);
    
    LCD_show_rec(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BACKGROUND_COLOR, interface->lcd.plcd);
    
    char title[100];
    if (result == DAO_SUCCESS) {
        snprintf(title, sizeof(title), "%s的养护记录", plant_info.name);
    } else {
        snprintf(title, sizeof(title), "养护记录 (ID: %d)", plant_id);
    }
    
    interface_draw_header(interface, title);
    
    Button add_button = {SCREEN_WIDTH - BUTTON_WIDTH - 20, HEADER_HEIGHT + 10, BUTTON_WIDTH, BUTTON_HEIGHT, "添加记录", BUTTON_COLOR, TEXT_COLOR};
    interface_draw_button(interface, &add_button);
    
    int start_y = HEADER_HEIGHT + BUTTON_HEIGHT + 30;
    int item_height = 60;
    
    // 获取养护记录数据
    interface_get_care_records(interface, plant_id);
    
    if (interface->care_record_count == 0) {
        const char *no_records = "暂无养护记录";
        int text_x = (SCREEN_WIDTH - strlen(no_records) * 8) / 2;
        int text_y = start_y + 50;
        
        showString(interface->lcd.plcd, no_records, 16, TEXT_COLOR, text_x, text_y, BACKGROUND_COLOR);
    } else {
        for (int i = 0; i < interface->care_record_count && i < 5; i++) {
            int y = start_y + i * item_height;
            
            char record_info[150];
            const char* type_str = care_type_to_string(interface->care_records[i].operation_type);
            snprintf(record_info, sizeof(record_info), "%s - %s", 
                     interface->care_records[i].operation_date, type_str);
            
            LCD_show_rec(20, y, SCREEN_WIDTH - 40, item_height - 5, 0xF5F5F5, interface->lcd.plcd);
            
            showString(interface->lcd.plcd, record_info, 16, TEXT_COLOR, 30, y + 10, 0xF5F5F5);
            
            // 显示操作详情（截取前一部分）
            char detail_preview[100];
            strncpy(detail_preview, interface->care_records[i].details, sizeof(detail_preview) - 1);
            detail_preview[sizeof(detail_preview) - 1] = '\0';
            
            // 如果详情太长，添加省略号
            if (strlen(interface->care_records[i].details) > sizeof(detail_preview) - 4) {
                strcpy(detail_preview + sizeof(detail_preview) - 4, "...");
            }
            
            showString(interface->lcd.plcd, detail_preview, 14, 0x666666, 30, y + 35, 0xF5F5F5);
        }
    }
    
    interface_draw_footer(interface);
}

void interface_draw_reminders(Interface *interface) {
    LCD_show_rec(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BACKGROUND_COLOR, interface->lcd.plcd);
    interface_draw_header(interface, "提醒管理");
    
    Button add_button = {SCREEN_WIDTH - BUTTON_WIDTH - 20, HEADER_HEIGHT + 10, BUTTON_WIDTH, BUTTON_HEIGHT, "添加提醒", BUTTON_COLOR, TEXT_COLOR};
    interface_draw_button(interface, &add_button);
    
    int start_y = HEADER_HEIGHT + BUTTON_HEIGHT + 30;
    int item_height = 70;
    
    // 获取提醒数据
    interface_get_reminders(interface);
    
    if (interface->reminder_count == 0) {
        const char *no_reminders = "暂无提醒设置";
        int text_x = (SCREEN_WIDTH - strlen(no_reminders) * 8) / 2;
        int text_y = start_y + 50;
        
        showString(interface->lcd.plcd, no_reminders, 16, TEXT_COLOR, text_x, text_y, BACKGROUND_COLOR);
    } else {
        for (int i = 0; i < interface->reminder_count && i < 5; i++) {
            int y = start_y + i * item_height;
            
            // 获取植物名称
            Plant plant_info;
            plant_init(&plant_info);
            DAO_RESULT result = plants_dao_get_by_id(interface->reminders[i].plant_id, &plant_info);
            
            char reminder_info[150];
            char plant_name[51];
            if (result == DAO_SUCCESS) {
                strncpy(plant_name, plant_info.name, sizeof(plant_name) - 1);
                plant_name[sizeof(plant_name) - 1] = '\0';
            } else {
                snprintf(plant_name, sizeof(plant_name), "ID:%d", interface->reminders[i].plant_id);
            }
            
            const char* type_str = reminder_type_to_string(interface->reminders[i].reminder_type);
            snprintf(reminder_info, sizeof(reminder_info), "%s - %s", plant_name, type_str);
            
            LCD_show_rec(20, y, SCREEN_WIDTH - 40, item_height - 5, 0xF5F5F5, interface->lcd.plcd);
            
            showString(interface->lcd.plcd, reminder_info, 16, TEXT_COLOR, 30, y + 10, 0xF5F5F5);
            
            // 显示频率和下次提醒日期
            char frequency_info[100];
            if (interface->reminders[i].is_active) {
                snprintf(frequency_info, sizeof(frequency_info), "频率: %d天  下次: %s", 
                         interface->reminders[i].frequency, interface->reminders[i].next_reminder_date);
            } else {
                snprintf(frequency_info, sizeof(frequency_info), "已停用  频率: %d天", 
                         interface->reminders[i].frequency);
            }
            
            showString(interface->lcd.plcd, frequency_info, 14, 0x666666, 30, y + 35, 0xF5F5F5);
        }
    }
    
    interface_draw_footer(interface);
}

void interface_navigate_to(Interface *interface, ScreenType screen) {
    interface->current_screen = screen;
    
    switch (screen) {
        case MAIN_MENU:
            interface_draw_main_menu(interface);
            break;
        case PLANT_LIST:
            interface_get_plants(interface);
            interface_draw_plant_list(interface);
            break;
        case PLANT_DETAIL:
            interface_draw_plant_detail(interface, interface->selected_plant_id);
            break;
        case CARE_RECORDS:
            interface_draw_care_records(interface, interface->selected_plant_id);
            break;
        case REMINDERS:
            interface_draw_reminders(interface);
            break;
        default:
            interface_draw_main_menu(interface);
            break;
    }
}