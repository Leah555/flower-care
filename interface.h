#ifndef INTERFACE_H
#define INTERFACE_H

#ifdef __linux__
#include <sys/mman.h>
#endif
#include "lcd.h"
#include "event.h"
#include "dao/plants_dao.h"
#include "dao/care_records_dao.h"
#include "dao/reminders_dao.h"

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
    REMINDERS,
    ADD_REMINDER,
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

typedef struct {
    ScreenType current_screen;
    LCD lcd;
    Plant *plants;
    int plant_count;
    int plants_capacity;
    CareRecord *care_records;
    int care_record_count;
    int care_records_capacity;
    Reminder *reminders;
    int reminder_count;
    int reminders_capacity;
    int selected_plant_id;
    int selected_care_record_id;
    int selected_reminder_id;
} Interface;

Interface* interface_init();
void interface_cleanup(Interface *interface);
void interface_draw_main_menu(Interface *interface);
void interface_draw_plant_list(Interface *interface);
void interface_draw_plant_detail(Interface *interface, int plant_id);
void interface_draw_care_records(Interface *interface, int plant_id);
void interface_draw_reminders(Interface *interface);
void interface_handle_touch(Interface *interface, struct point touch_point);
void interface_navigate_to(Interface *interface, ScreenType screen);
void interface_draw_button(Interface *interface, Button *button);
int interface_is_point_in_button(struct point p, Button button);
void interface_draw_header(Interface *interface, const char *title);
void interface_draw_footer(Interface *interface);

#endif