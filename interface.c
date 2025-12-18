#include "interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "font_1.h"
#include "dao/growth_records_dao.h"
#include "dao/care_experience_dao.h"

// 全局接口实例指针定义
Interface* interface_instance = NULL;

static int interface_get_plants(Interface *interface);
static int interface_get_care_records(Interface *interface, int plant_id);
static int interface_get_growth_records(Interface *interface, int plant_id);
static int interface_get_reminders(Interface *interface);
static int interface_get_care_experiences(Interface *interface);
static CareExperience* interface_find_experience_by_variety(Interface *interface, const char *variety);

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

// 生长记录数据收集回调函数
static void growth_records_collect_callback(const GrowthRecord* record) {
    if (!interface_instance || !record) return;
    
    Interface *interface = interface_instance;
    
    if (interface->growth_record_count >= interface->growth_records_capacity) {
        interface->growth_records_capacity += 10; // 每次增加10个容量
        GrowthRecord *temp = realloc(interface->growth_records, interface->growth_records_capacity * sizeof(GrowthRecord));
        if (!temp) return; // 内存分配失败
        interface->growth_records = temp;
    }
    
    // 复制生长记录数据
    memcpy(&interface->growth_records[interface->growth_record_count], record, sizeof(GrowthRecord));
    interface->growth_record_count++;
}

// 养护经验数据收集回调函数
static void care_experiences_collect_callback(const CareExperience* experience) {
    if (!interface_instance || !experience) return;
    
    Interface *interface = interface_instance;
    
    if (interface->care_experience_count >= interface->care_experiences_capacity) {
        interface->care_experiences_capacity += 10; // 每次增加10个容量
        CareExperience *temp = realloc(interface->care_experiences, interface->care_experiences_capacity * sizeof(CareExperience));
        if (!temp) return; // 内存分配失败
        interface->care_experiences = temp;
    }
    
    // 复制养护经验数据
    memcpy(&interface->care_experiences[interface->care_experience_count], experience, sizeof(CareExperience));
    interface->care_experience_count++;
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
    interface->growth_records = NULL;
    interface->growth_record_count = 0;
    interface->growth_records_capacity = 0;
    interface->care_experiences = NULL;
    interface->care_experience_count = 0;
    interface->care_experiences_capacity = 0;
    interface->selected_plant_id = -1;
    interface->selected_care_record_id = -1;
    interface->selected_growth_record_id = -1;
    interface->selected_reminder_id = -1;
    interface->selected_experience_id = -1;
    
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
    if (interface->growth_records) {
        free(interface->growth_records);
    }
    if (interface->care_experiences) {
        free(interface->care_experiences);
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
    
    // 使用更小的按钮以适应更多选项
    int small_button_height = 50;
    int small_button_width = 180;
    int small_margin = 15;
    
    Button buttons[] = {
        {(SCREEN_WIDTH - small_button_width) / 2, HEADER_HEIGHT + small_margin, small_button_width, small_button_height, "植物管理", BUTTON_COLOR, TEXT_COLOR},
        {(SCREEN_WIDTH - small_button_width) / 2, HEADER_HEIGHT + small_margin * 2 + small_button_height, small_button_width, small_button_height, "养护记录", BUTTON_COLOR, TEXT_COLOR},
        {(SCREEN_WIDTH - small_button_width) / 2, HEADER_HEIGHT + small_margin * 3 + small_button_height * 2, small_button_width, small_button_height, "生长记录", BUTTON_COLOR, TEXT_COLOR},
        {(SCREEN_WIDTH - small_button_width) / 2, HEADER_HEIGHT + small_margin * 4 + small_button_height * 3, small_button_width, small_button_height, "提醒管理", BUTTON_COLOR, TEXT_COLOR},
        {(SCREEN_WIDTH - small_button_width) / 2, HEADER_HEIGHT + small_margin * 5 + small_button_height * 4, small_button_width, small_button_height, "养护经验", BUTTON_COLOR, TEXT_COLOR},
        {(SCREEN_WIDTH - small_button_width) / 2, HEADER_HEIGHT + small_margin * 6 + small_button_height * 5, small_button_width, small_button_height, "系统设置", BUTTON_COLOR, TEXT_COLOR}
    };
    
    for (int i = 0; i < 6; i++) {
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
    
    // 显示养护经验（如果存在）
    CareExperience* experience = interface_find_experience_by_variety(interface, plant->variety);
    if (experience) {
        y += 30;
        showString(interface->lcd.plcd, "--- 养护经验 ---", 14, HIGHLIGHT_COLOR, 30, y, BACKGROUND_COLOR);
        y += 25;
        char exp_info[150];
        snprintf(exp_info, sizeof(exp_info), "最佳浇水: %d天 | 成功率: %.1f%%", 
                 experience->optimal_water_frequency, experience->success_rate);
        showString(interface->lcd.plcd, exp_info, 14, TEXT_COLOR, 30, y, BACKGROUND_COLOR);
    }
    
    y += 40;
    Button care_button = {30, y, BUTTON_WIDTH, BUTTON_HEIGHT, "养护记录", BUTTON_COLOR, TEXT_COLOR};
    interface_draw_button(interface, &care_button);
    
    Button growth_button = {30 + BUTTON_WIDTH + 20, y, BUTTON_WIDTH, BUTTON_HEIGHT, "生长记录", BUTTON_COLOR, TEXT_COLOR};
    interface_draw_button(interface, &growth_button);
    
    y += BUTTON_HEIGHT + 10;
    Button edit_button = {30, y, BUTTON_WIDTH, BUTTON_HEIGHT, "编辑植物", BUTTON_COLOR, TEXT_COLOR};
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
            
            int small_button_height = 50;
            int small_button_width = 180;
            int small_margin = 15;
            int button_y_start = HEADER_HEIGHT + small_margin;
            
            for (int i = 0; i < 6; i++) {
                Button test_button = {(SCREEN_WIDTH - small_button_width) / 2, button_y_start + i * (small_button_height + small_margin), small_button_width, small_button_height, "", 0, 0};
                
                if (interface_is_point_in_button(touch_point, test_button)) {
                    switch (i) {
                        case 0: interface_navigate_to(interface, PLANT_LIST); break;
                        case 1: 
                            // 养护记录需要先选择植物
                            if (interface->plant_count == 0) {
                                interface_get_plants(interface);
                            }
                            if (interface->plant_count > 0) {
                                interface->selected_plant_id = interface->plants[0].id;
                                interface_navigate_to(interface, CARE_RECORDS);
                            }
                            break;
                        case 2:
                            // 生长记录需要先选择植物
                            if (interface->plant_count == 0) {
                                interface_get_plants(interface);
                            }
                            if (interface->plant_count > 0) {
                                interface->selected_plant_id = interface->plants[0].id;
                                interface_navigate_to(interface, GROWTH_RECORDS);
                            }
                            break;
                        case 3: interface_navigate_to(interface, REMINDERS); break;
                        case 4: interface_navigate_to(interface, CARE_EXPERIENCE); break;
                        case 5: interface_navigate_to(interface, SETTINGS); break;
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
            
            Button add_reminder_btn = {SCREEN_WIDTH - BUTTON_WIDTH - 20, HEADER_HEIGHT + 10, BUTTON_WIDTH, BUTTON_HEIGHT, "", 0, 0};
            if (interface_is_point_in_button(touch_point, add_reminder_btn)) {
                int plant_id = -1;
                if (interface->selected_plant_id > 0) {
                    plant_id = interface->selected_plant_id;
                } else {
                    if (interface->plant_count == 0) {
                        interface_get_plants(interface);
                    }
                    if (interface->plant_count > 0) {
                        plant_id = interface->plants[0].id;
                    } else {
                        return;
                    }
                }

                Reminder reminder;
                reminder_init(&reminder);
                reminder.plant_id = plant_id;
                reminder.reminder_type = REMINDER_WATER;
                reminder.frequency = 3;
                reminder.is_active = 1;
                time_t t = time(NULL);
                struct tm *tm_info = localtime(&t);
                char date_str[11];
                strftime(date_str, 11, "%Y-%m-%d", tm_info);
                strncpy(reminder.last_reminder_date, date_str, sizeof(reminder.last_reminder_date) - 1);
                reminder.last_reminder_date[sizeof(reminder.last_reminder_date) - 1] = '\0';

                DAO_RESULT r = reminders_dao_add(&reminder);
                if (r == DAO_SUCCESS) {
                    interface_get_reminders(interface);
                    interface_draw_reminders(interface);
                }
                return;
            }
            
            // Check if any reminder item was pressed
            int reminder_start_y = HEADER_HEIGHT + BUTTON_HEIGHT + 30;
            int reminder_item_height = 70;
            
            for (int i = 0; i < interface->reminder_count && i < 5; i++) {
                int y = reminder_start_y + i * reminder_item_height;
                Button reminder_item_btn = {20, y, SCREEN_WIDTH - 40, reminder_item_height - 5, "", 0, 0};
                
                if (interface_is_point_in_button(touch_point, reminder_item_btn)) {
                    int rid = interface->reminders[i].id;
                    int current_active = interface->reminders[i].is_active;
                    DAO_RESULT r = reminders_dao_set_active(rid, current_active ? 0 : 1);
                    if (r == DAO_SUCCESS) {
                        interface_get_reminders(interface);
                        interface_draw_reminders(interface);
                    }
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
                Plant plant;
                plant_init(&plant);
                strncpy(plant.name, "新植物", sizeof(plant.name) - 1);
                plant.name[sizeof(plant.name) - 1] = '\0';
                strncpy(plant.variety, "未知品种", sizeof(plant.variety) - 1);
                plant.variety[sizeof(plant.variety) - 1] = '\0';
                strncpy(plant.planting_date, "2025-01-01", sizeof(plant.planting_date) - 1);
                plant.planting_date[sizeof(plant.planting_date) - 1] = '\0';
                plant.water_frequency = 3;
                strncpy(plant.last_water_date, "2025-01-01", sizeof(plant.last_water_date) - 1);
                plant.last_water_date[sizeof(plant.last_water_date) - 1] = '\0';
                strncpy(plant.last_fertilize_date, "2025-01-01", sizeof(plant.last_fertilize_date) - 1);
                plant.last_fertilize_date[sizeof(plant.last_fertilize_date) - 1] = '\0';
                strncpy(plant.status, "正常", sizeof(plant.status) - 1);
                plant.status[sizeof(plant.status) - 1] = '\0';
                strncpy(plant.notes, "", sizeof(plant.notes) - 1);
                plant.notes[sizeof(plant.notes) - 1] = '\0';
                DAO_RESULT r = plants_dao_add(&plant);
                if (r == DAO_SUCCESS) {
                    interface_get_plants(interface);
                    interface_draw_plant_list(interface);
                }
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
            
            // 计算按钮位置（考虑可能显示的养护经验）
            Plant *plant = NULL;
            for (int i = 0; i < interface->plant_count; i++) {
                if (interface->plants[i].id == interface->selected_plant_id) {
                    plant = &interface->plants[i];
                    break;
                }
            }
            
            int detail_y = HEADER_HEIGHT + 20;
            for (int i = 0; i < 6; i++) { // 跳过6行信息显示
                detail_y += 30;
            }
            
            // 如果有养护经验，需要额外空间
            if (plant) {
                CareExperience* experience = interface_find_experience_by_variety(interface, plant->variety);
                if (experience) {
                    detail_y += 55; // 养护经验显示区域
                }
            }
            
            Button care_button = {30, detail_y + 40, BUTTON_WIDTH, BUTTON_HEIGHT, "", 0, 0};
            Button growth_button = {30 + BUTTON_WIDTH + 20, detail_y + 40, BUTTON_WIDTH, BUTTON_HEIGHT, "", 0, 0};
            Button edit_button = {30, detail_y + BUTTON_HEIGHT + 50, BUTTON_WIDTH, BUTTON_HEIGHT, "", 0, 0};
            
            if (interface_is_point_in_button(touch_point, care_button)) {
                interface_navigate_to(interface, CARE_RECORDS);
                return;
            }
            
            if (interface_is_point_in_button(touch_point, growth_button)) {
                interface_navigate_to(interface, GROWTH_RECORDS);
                return;
            }
            
            if (interface_is_point_in_button(touch_point, edit_button)) {
                if (interface->selected_plant_id > 0) {
                    Plant p;
                    plant_init(&p);
                    if (plants_dao_get_by_id(interface->selected_plant_id, &p) == DAO_SUCCESS) {
                        if (strcmp(p.status, "正常") == 0) {
                            strncpy(p.status, "良好", sizeof(p.status) - 1);
                            p.status[sizeof(p.status) - 1] = '\0';
                        } else {
                            strncpy(p.status, "正常", sizeof(p.status) - 1);
                            p.status[sizeof(p.status) - 1] = '\0';
                        }
                        plants_dao_update(interface->selected_plant_id, &p);
                        interface_get_plants(interface);
                        interface_draw_plant_detail(interface, interface->selected_plant_id);
                    }
                }
                return;
            }
            break;

        case CARE_RECORDS:
            if (touch_point.y >= SCREEN_HEIGHT - FOOTER_HEIGHT) {
                interface_navigate_to(interface, PLANT_DETAIL);
                return;
            }

            {
                Button add_record_btn = {SCREEN_WIDTH - BUTTON_WIDTH - 20, HEADER_HEIGHT + 10, BUTTON_WIDTH, BUTTON_HEIGHT, "", 0, 0};
                if (interface_is_point_in_button(touch_point, add_record_btn)) {
                    if (interface->selected_plant_id > 0) {
                        CareRecord record;
                        care_record_init(&record);
                        record.plant_id = interface->selected_plant_id;
                        record.operation_type = CARE_WATER;
                        time_t t = time(NULL);
                        struct tm *tm_info = localtime(&t);
                        char datetime_str[20];
                        strftime(datetime_str, 20, "%Y-%m-%d %H:%M:%S", tm_info);
                        strncpy(record.operation_date, datetime_str, sizeof(record.operation_date) - 1);
                        record.operation_date[sizeof(record.operation_date) - 1] = '\0';
                        strncpy(record.details, "自动添加的养护记录", sizeof(record.details) - 1);
                        record.details[sizeof(record.details) - 1] = '\0';
                        strncpy(record.amount, "500ml", sizeof(record.amount) - 1);
                        record.amount[sizeof(record.amount) - 1] = '\0';
                        DAO_RESULT r = care_records_dao_add(&record);
                        if (r == DAO_SUCCESS) {
                            interface_get_care_records(interface, interface->selected_plant_id);
                            interface_draw_care_records(interface, interface->selected_plant_id);
                        }
                    }
                    return;
                }
            }
            break;
            
        case GROWTH_RECORDS:
            if (touch_point.y >= SCREEN_HEIGHT - FOOTER_HEIGHT) {
                interface_navigate_to(interface, PLANT_DETAIL);
                return;
            }

            {
                Button add_record_btn = {SCREEN_WIDTH - BUTTON_WIDTH - 20, HEADER_HEIGHT + 10, BUTTON_WIDTH, BUTTON_HEIGHT, "", 0, 0};
                if (interface_is_point_in_button(touch_point, add_record_btn)) {
                    if (interface->selected_plant_id > 0) {
                        GrowthRecord record;
                        growth_record_init(&record);
                        record.plant_id = interface->selected_plant_id;
                        time_t t = time(NULL);
                        struct tm *tm_info = localtime(&t);
                        char date_str[11];
                        strftime(date_str, 11, "%Y-%m-%d", tm_info);
                        strncpy(record.record_date, date_str, sizeof(record.record_date) - 1);
                        record.record_date[sizeof(record.record_date) - 1] = '\0';
                        record.height_cm = 25.0;
                        record.leaf_count = 10;
                        record.bud_count = 2;
                        record.flower_count = 1;
                        record.health_score = 80;
                        strncpy(record.leaf_color, "绿色", sizeof(record.leaf_color) - 1);
                        record.leaf_color[sizeof(record.leaf_color) - 1] = '\0';
                        strncpy(record.growth_vigor, "良好", sizeof(record.growth_vigor) - 1);
                        record.growth_vigor[sizeof(record.growth_vigor) - 1] = '\0';
                        record.temperature = 22;
                        record.humidity = 65;
                        strncpy(record.light_exposure, "充足", sizeof(record.light_exposure) - 1);
                        record.light_exposure[sizeof(record.light_exposure) - 1] = '\0';
                        strncpy(record.notes, "自动添加的生长记录", sizeof(record.notes) - 1);
                        record.notes[sizeof(record.notes) - 1] = '\0';
                        DAO_RESULT r = growth_records_dao_add(&record);
                        if (r == DAO_SUCCESS) {
                            interface_get_growth_records(interface, interface->selected_plant_id);
                            interface_draw_growth_records(interface, interface->selected_plant_id);
                        }
                    }
                    return;
                }
            }
            break;
            
        case CARE_EXPERIENCE:
            if (touch_point.y >= SCREEN_HEIGHT - FOOTER_HEIGHT) {
                interface_navigate_to(interface, MAIN_MENU);
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

// 获取特定植物的生长记录
static int interface_get_growth_records(Interface *interface, int plant_id) {
    // 重置计数器
    interface->growth_record_count = 0;
    
    // 确保有足够的容量
    if (interface->growth_records_capacity < 10) {
        interface->growth_records_capacity = 10;
        interface->growth_records = realloc(interface->growth_records, interface->growth_records_capacity * sizeof(GrowthRecord));
    }
    
    // 保存接口实例以便在回调函数中使用
    interface_instance = interface;
    
    // 调用DAO层获取指定植物的所有生长记录
    DAO_RESULT result = growth_records_dao_get_by_plant(plant_id, growth_records_collect_callback);
    
    if (result != DAO_SUCCESS) {
        printf("获取生长记录失败\n");
        return -1;
    }
    
    return interface->growth_record_count;
}

// 获取养护经验数据
static int interface_get_care_experiences(Interface *interface) {
    // 重置计数器
    interface->care_experience_count = 0;
    
    // 确保有足够的容量
    if (interface->care_experiences_capacity < 10) {
        interface->care_experiences_capacity = 10;
        interface->care_experiences = realloc(interface->care_experiences, interface->care_experiences_capacity * sizeof(CareExperience));
    }
    
    // 保存接口实例以便在回调函数中使用
    interface_instance = interface;
    
    // 调用DAO层获取所有养护经验数据
    DAO_RESULT result = care_experience_dao_get_all(care_experiences_collect_callback);
    
    if (result != DAO_SUCCESS) {
        printf("获取养护经验数据失败\n");
        return -1;
    }
    
    return interface->care_experience_count;
}

// 根据品种查找养护经验
static CareExperience* interface_find_experience_by_variety(Interface *interface, const char *variety) {
    if (!variety || strlen(variety) == 0) return NULL;
    
    // 确保已加载养护经验数据
    if (interface->care_experience_count == 0) {
        interface_get_care_experiences(interface);
    }
    
    // 查找匹配的养护经验
    for (int i = 0; i < interface->care_experience_count; i++) {
        if (strcmp(interface->care_experiences[i].variety, variety) == 0) {
            return &interface->care_experiences[i];
        }
    }
    
    return NULL;
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

void interface_draw_growth_records(Interface *interface, int plant_id) {
    // 获取植物信息用于标题显示
    Plant plant_info;
    plant_init(&plant_info);
    DAO_RESULT result = plants_dao_get_by_id(plant_id, &plant_info);
    
    LCD_show_rec(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BACKGROUND_COLOR, interface->lcd.plcd);
    
    char title[100];
    if (result == DAO_SUCCESS) {
        snprintf(title, sizeof(title), "%s的生长记录", plant_info.name);
    } else {
        snprintf(title, sizeof(title), "生长记录 (ID: %d)", plant_id);
    }
    
    interface_draw_header(interface, title);
    
    Button add_button = {SCREEN_WIDTH - BUTTON_WIDTH - 20, HEADER_HEIGHT + 10, BUTTON_WIDTH, BUTTON_HEIGHT, "添加记录", BUTTON_COLOR, TEXT_COLOR};
    interface_draw_button(interface, &add_button);
    
    int start_y = HEADER_HEIGHT + BUTTON_HEIGHT + 30;
    int item_height = 80;
    
    // 获取生长记录数据
    interface_get_growth_records(interface, plant_id);
    
    if (interface->growth_record_count == 0) {
        const char *no_records = "暂无生长记录";
        int text_x = (SCREEN_WIDTH - strlen(no_records) * 8) / 2;
        int text_y = start_y + 50;
        
        showString(interface->lcd.plcd, no_records, 16, TEXT_COLOR, text_x, text_y, BACKGROUND_COLOR);
    } else {
        for (int i = 0; i < interface->growth_record_count && i < 4; i++) {
            int y = start_y + i * item_height;
            
            LCD_show_rec(20, y, SCREEN_WIDTH - 40, item_height - 5, 0xF5F5F5, interface->lcd.plcd);
            
            // 显示记录日期
            char date_info[100];
            snprintf(date_info, sizeof(date_info), "%s", interface->growth_records[i].record_date);
            showString(interface->lcd.plcd, date_info, 16, TEXT_COLOR, 30, y + 5, 0xF5F5F5);
            
            // 显示生长指标
            char growth_info[150];
            snprintf(growth_info, sizeof(growth_info), "高度: %.1fcm | 叶片: %d | 花朵: %d | 健康: %d分", 
                     interface->growth_records[i].height_cm,
                     interface->growth_records[i].leaf_count,
                     interface->growth_records[i].flower_count,
                     interface->growth_records[i].health_score);
            showString(interface->lcd.plcd, growth_info, 14, 0x666666, 30, y + 25, 0xF5F5F5);
            
            // 显示环境信息
            char env_info[150];
            snprintf(env_info, sizeof(env_info), "温度: %d°C | 湿度: %d%% | 光照: %s", 
                     interface->growth_records[i].temperature,
                     interface->growth_records[i].humidity,
                     interface->growth_records[i].light_exposure);
            showString(interface->lcd.plcd, env_info, 14, 0x666666, 30, y + 45, 0xF5F5F5);
        }
    }
    
    interface_draw_footer(interface);
}

void interface_draw_care_experience(Interface *interface) {
    LCD_show_rec(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BACKGROUND_COLOR, interface->lcd.plcd);
    interface_draw_header(interface, "养护经验库");
    
    // 获取养护经验数据
    interface_get_care_experiences(interface);
    
    int start_y = HEADER_HEIGHT + 20;
    int item_height = 120;
    
    if (interface->care_experience_count == 0) {
        const char *no_experiences = "暂无养护经验";
        int text_x = (SCREEN_WIDTH - strlen(no_experiences) * 8) / 2;
        int text_y = start_y + 50;
        
        showString(interface->lcd.plcd, no_experiences, 16, TEXT_COLOR, text_x, text_y, BACKGROUND_COLOR);
    } else {
        for (int i = 0; i < interface->care_experience_count && i < 3; i++) {
            int y = start_y + i * item_height;
            
            LCD_show_rec(20, y, SCREEN_WIDTH - 40, item_height - 5, 0xF5F5F5, interface->lcd.plcd);
            
            // 显示品种名称
            char variety_info[100];
            snprintf(variety_info, sizeof(variety_info), "品种: %s (置信度: %d/5)", 
                     interface->care_experiences[i].variety,
                     interface->care_experiences[i].confidence_level);
            showString(interface->lcd.plcd, variety_info, 16, TEXT_COLOR, 30, y + 5, 0xF5F5F5);
            
            // 显示最佳养护频率
            char frequency_info[150];
            snprintf(frequency_info, sizeof(frequency_info), "浇水: %d天 | 施肥: %d天 | 最佳季节: %s", 
                     interface->care_experiences[i].optimal_water_frequency,
                     interface->care_experiences[i].optimal_fertilize_frequency,
                     interface->care_experiences[i].best_season);
            showString(interface->lcd.plcd, frequency_info, 14, 0x666666, 30, y + 25, 0xF5F5F5);
            
            // 显示统计信息
            char stats_info[150];
            snprintf(stats_info, sizeof(stats_info), "成功率: %.1f%% | 平均健康: %.1f | 养护数量: %d", 
                     interface->care_experiences[i].success_rate,
                     interface->care_experiences[i].avg_health_score,
                     interface->care_experiences[i].total_plants);
            showString(interface->lcd.plcd, stats_info, 14, 0x666666, 30, y + 45, 0xF5F5F5);
            
            // 显示常见问题
            char problems_info[150];
            strncpy(problems_info, interface->care_experiences[i].common_mistakes, sizeof(problems_info) - 1);
            problems_info[sizeof(problems_info) - 1] = '\0';
            if (strlen(interface->care_experiences[i].common_mistakes) > 30) {
                strcpy(problems_info + 27, "...");
            }
            showString(interface->lcd.plcd, problems_info, 12, 0x888888, 30, y + 65, 0xF5F5F5);
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
            // 确保已加载养护经验数据以便显示
            interface_get_care_experiences(interface);
            interface_draw_plant_detail(interface, interface->selected_plant_id);
            break;
        case CARE_RECORDS:
            interface_draw_care_records(interface, interface->selected_plant_id);
            break;
        case GROWTH_RECORDS:
            interface_draw_growth_records(interface, interface->selected_plant_id);
            break;
        case REMINDERS:
            interface_draw_reminders(interface);
            break;
        case CARE_EXPERIENCE:
            interface_draw_care_experience(interface);
            break;
        default:
            interface_draw_main_menu(interface);
            break;
    }
}
