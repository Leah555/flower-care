#include "interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "font_1.h"

// 植物数据收集回调函数
static void plants_collect_callback(const Plant* plant) {
    // 这个函数将在DAO层被调用，用于收集植物数据
    // 实际的数据收集将在interface_navigate_to函数中实现
}

// 养护记录数据收集回调函数
static void care_records_collect_callback(const CareRecord* record) {
    // 这个函数将在DAO层被调用，用于收集养护记录数据
}

// 提醒数据收集回调函数
static void reminders_collect_callback(const Reminder* reminder) {
    // 这个函数将在DAO层被调用，用于收集提醒数据
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
    
    interface->lcd = LCD_init();
    if (interface->lcd.fd == -1) {
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
            
        default:
            if (touch_point.y >= SCREEN_HEIGHT - FOOTER_HEIGHT) {
                interface_navigate_to(interface, MAIN_MENU);
            }
            break;
    }
}

// 简化版本的数据获取函数
static int interface_get_plants(Interface *interface) {
    // 由于DAO层使用回调模式，这里我们简化处理
    // 在实际项目中，需要实现完整的数据收集逻辑
    interface->plant_count = 0;
    
    // 模拟一些测试数据用于界面显示
    if (interface->plants_capacity < 3) {
        interface->plants = realloc(interface->plants, 3 * sizeof(Plant));
        interface->plants_capacity = 3;
    }
    
    // 添加测试数据
    if (interface->plants_capacity >= 1) {
        Plant *plant = &interface->plants[0];
        memset(plant, 0, sizeof(Plant));
        plant->id = 1;
        strcpy(plant->name, "玫瑰");
        strcpy(plant->variety, "红玫瑰");
        strcpy(plant->planting_date, "2024-01-15");
        plant->water_frequency = 3;
        strcpy(plant->last_water_date, "2024-12-18");
        strcpy(plant->last_fertilize_date, "2024-12-10");
        strcpy(plant->status, "正常");
        interface->plant_count++;
    }
    
    if (interface->plants_capacity >= 2) {
        Plant *plant = &interface->plants[1];
        memset(plant, 0, sizeof(Plant));
        plant->id = 2;
        strcpy(plant->name, "兰花");
        strcpy(plant->variety, "蝴蝶兰");
        strcpy(plant->planting_date, "2024-03-20");
        plant->water_frequency = 7;
        strcpy(plant->last_water_date, "2024-12-17");
        strcpy(plant->last_fertilize_date, "2024-12-05");
        strcpy(plant->status, "良好");
        interface->plant_count++;
    }
    
    return interface->plant_count;
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
            // 简化处理，暂时不实现
            interface_draw_main_menu(interface);
            break;
        case REMINDERS:
            // 简化处理，暂时不实现
            interface_draw_main_menu(interface);
            break;
        default:
            interface_draw_main_menu(interface);
            break;
    }
}