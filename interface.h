#ifndef INTERFACE_H
#define INTERFACE_H

#ifdef __linux__
#include <sys/mman.h>
#endif
#include "lcd.h"
#include "event.h"
#include "dao/dao.h"
#include "dao/plants_dao.h"
#include "dao/care_records_dao.h"
#include "dao/reminders_dao.h"
#include "dao/growth_records_dao.h"
#include "dao/care_experience_dao.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 480
#define BUTTON_WIDTH 200
#define BUTTON_HEIGHT 60
#define BUTTON_MARGIN 20
#define HEADER_HEIGHT 80
#define FOOTER_HEIGHT 60

typedef enum {
    MAIN_MENU,
    PLANT_LIST,
    PLANT_DETAIL,
    ADD_PLANT,
    CARE_RECORDS,
    ADD_CARE_RECORD,
    GROWTH_RECORDS,
    ADD_GROWTH_RECORD,
    REMINDERS,
    ADD_REMINDER,
    CARE_EXPERIENCE,
    SETTINGS
} ScreenType;

typedef struct Button {
    int pos_x;
    int pos_y;
    int btn_width;
    int btn_height;
    char *btn_text;
    int btn_color;
    int btn_text_color;
} Button;

// 前向声明Interface结构体
typedef struct Interface Interface;

// 全局接口实例指针，用于回调函数访问
extern Interface* interface_instance;

typedef struct Interface {
    ScreenType current_screen;
    LCD lcd;
    Plant *plants;
    int plant_count;
    int plants_capacity;
    CareRecord *care_records;
    int care_record_count;
    int care_records_capacity;
    GrowthRecord *growth_records;
    int growth_record_count;
    int growth_records_capacity;
    Reminder *reminders;
    int reminder_count;
    int reminders_capacity;
    CareExperience *care_experiences;
    int care_experience_count;
    int care_experiences_capacity;
    int selected_plant_id;
    int selected_care_record_id;
    int selected_growth_record_id;
    int selected_reminder_id;
    int selected_experience_id;
} Interface;

Interface* interface_init();
void interface_cleanup(Interface *interface);
void interface_draw_main_menu(Interface *interface);
void interface_draw_plant_list(Interface *interface);
void interface_draw_plant_detail(Interface *interface, int plant_id);
void interface_draw_care_records(Interface *interface, int plant_id);
void interface_draw_growth_records(Interface *interface, int plant_id);
void interface_draw_reminders(Interface *interface);
void interface_draw_care_experience(Interface *interface);
void interface_handle_touch(Interface *interface, struct point touch_point);
void interface_navigate_to(Interface *interface, ScreenType screen);
void interface_draw_button(Interface *interface, Button *button);
int interface_is_point_in_button(struct point p, Button button);
void interface_draw_header(Interface *interface, const char *title);
void interface_draw_footer(Interface *interface);

#endif