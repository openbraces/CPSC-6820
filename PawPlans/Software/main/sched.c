#include <esp_log.h>
#include <esp_timer.h>
#include <stdlib.h>
#include <sys/time.h>

#include "clock.h"
#include "hardware.h"
#include "io.h"
#include "sched.h"
#include "ui.h"

#define TAG                  "sched"
#define MAX_SCHEDULES        10
#define FOOD_LEVEL_THRESHOLD 30

static lv_obj_t   *sched_root;
static lv_obj_t   *sched_cont;
static lv_obj_t   *roller_hour;
static lv_obj_t   *roller_min;
static lv_obj_t   *sb_portion;
static lv_group_t *sched_group;

typedef struct {
  int hour;
  int min;
  int portion;
} schedule_entry_t;

static schedule_entry_t schedules[MAX_SCHEDULES];
static int              schedule_count = 0;
static int              editing_index  = -1;

static esp_timer_handle_t sched_timer = NULL;

static void sched_show_list(void);
static void sched_show_editor(int hour, int min);
static void sched_arm_next(void);
static void sched_trigger_action(void);
static void show_warning(const char *title, const char *msg);

static void cb_sched_new_btn(lv_event_t *e) {
  editing_index = -1;
  sched_show_editor(12, 0);
}

static void cb_sched_edit_btn(lv_event_t *e) {
  schedule_entry_t *entry = lv_event_get_user_data(e);
  editing_index           = entry - schedules;
  if (editing_index >= 0 && editing_index < schedule_count) {
    sched_show_editor(entry->hour, entry->min);
  }
}

static void cb_sched_set_btn(lv_event_t *e) {
  int hour = lv_roller_get_selected(roller_hour);
  int min  = lv_roller_get_selected(roller_min);

  for (int i = 0; i < schedule_count; ++i) {
    if (schedules[i].hour == hour && schedules[i].min == min) {
      show_warning("Duplicate Entry", "An entry with this time already exists");
      return;
    }
  }

  int portion = lv_spinbox_get_value(sb_portion);

  if (editing_index < 0 && schedule_count < MAX_SCHEDULES) {
    schedule_entry_t *entry = &schedules[schedule_count];
    entry->hour             = hour;
    entry->min              = min;
    entry->portion          = portion;
    ++schedule_count;
  } else if (editing_index >= 0) {
    schedule_entry_t *entry = &schedules[editing_index];
    entry->hour             = hour;
    entry->min              = min;
    entry->portion          = portion;
  }

  sched_arm_next();
  sched_show_list();
}

static void cb_sched_delete_btn(lv_event_t *e) {
  if (editing_index >= 0 && editing_index < schedule_count) {
    for (int i = editing_index; i < schedule_count - 1; ++i) {
      schedules[i] = schedules[i + 1];
    }
    --schedule_count;
  }

  sched_arm_next();
  sched_show_list();
}

static void cb_sched_back_btn(lv_event_t *e) { sched_show_list(); }

static void cb_sched_main_back_btn(lv_event_t *e) { ui_back_main(); }

static void sched_show_editor(int hour, int min) {
  lv_obj_clean(sched_root);

  lv_obj_t *sched_editor = lv_obj_create(sched_root);
  lv_obj_set_size(sched_editor, LV_PCT(100), LV_PCT(100));
  lv_obj_set_flex_flow(sched_editor, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_style_pad_all(sched_editor, 5, 0);
  lv_obj_set_scrollbar_mode(sched_editor, LV_SCROLLBAR_MODE_OFF);

  lv_obj_t *title = lv_label_create(sched_editor);
  lv_label_set_text(title, editing_index >= 0 ? "Edit Entry" : "New Entry");
  lv_obj_set_style_pad_hor(title, 5, 0);

  lv_obj_t *rollers_cont = lv_obj_create(sched_editor);
  lv_obj_set_width(rollers_cont, LV_PCT(100));
  lv_obj_set_flex_grow(rollers_cont, 1);
  lv_obj_set_flex_flow(rollers_cont, LV_FLEX_FLOW_ROW);
  lv_obj_set_style_pad_all(rollers_cont, 5, 0);
  lv_obj_set_style_pad_gap(rollers_cont, 10, 0);
  lv_obj_set_flex_align(rollers_cont, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

  roller_hour = lv_roller_create(rollers_cont);
  lv_roller_set_options(roller_hour, CLOCK_HOUR_OPTIONS, LV_ROLLER_MODE_NORMAL);
  lv_roller_set_selected(roller_hour, hour, LV_ANIM_ON);
  lv_obj_set_flex_grow(roller_hour, 1);
  lv_group_add_obj(sched_group, roller_hour);

  // Colon separator
  lv_obj_t *colon = lv_label_create(rollers_cont);
  lv_label_set_text(colon, ":");

  roller_min = lv_roller_create(rollers_cont);
  lv_roller_set_options(roller_min, CLOCK_MINUTE_OPTIONS,
                        LV_ROLLER_MODE_NORMAL);
  lv_roller_set_selected(roller_min, min, LV_ANIM_ON);
  lv_obj_set_flex_grow(roller_min, 1);
  lv_group_add_obj(sched_group, roller_min);

  sb_portion = lv_spinbox_create(rollers_cont);
  lv_spinbox_set_range(sb_portion, 0, 1000); // TODO: Set to max portion
  lv_spinbox_set_digit_format(sb_portion, 4, 0);
  lv_spinbox_set_value(sb_portion, 0);
  lv_obj_set_flex_grow(sb_portion, 1);
  lv_group_add_obj(sched_group, sb_portion);

  lv_obj_t *btn_cont = lv_obj_create(sched_editor);
  lv_obj_set_size(btn_cont, LV_PCT(100), LV_SIZE_CONTENT);
  lv_obj_set_flex_flow(btn_cont, LV_FLEX_FLOW_ROW);
  lv_obj_set_style_pad_all(btn_cont, 5, 0);

  lv_obj_t *btn_set = lv_btn_create(btn_cont);
  lv_obj_set_style_pad_all(btn_set, 2, 0);
  lv_obj_set_flex_grow(btn_set, 1);
  lv_obj_t *lbl_set = lv_label_create(btn_set);
  lv_label_set_text(lbl_set, "Set");
  lv_obj_center(lbl_set);
  lv_obj_add_event_cb(btn_set, cb_sched_set_btn, LV_EVENT_CLICKED, NULL);
  lv_group_add_obj(sched_group, btn_set);

  if (editing_index >= 0) {
    lv_obj_t *btn_delete = lv_btn_create(btn_cont);
    lv_obj_set_style_pad_all(btn_delete, 2, 0);
    lv_obj_set_flex_grow(btn_delete, 1);
    lv_obj_t *lbl_delete = lv_label_create(btn_delete);
    lv_label_set_text(lbl_delete, "Delete");
    lv_obj_center(lbl_delete);
    lv_obj_add_event_cb(btn_delete, cb_sched_delete_btn, LV_EVENT_CLICKED,
                        NULL);
    lv_group_add_obj(sched_group, btn_delete);
  }

  lv_obj_t *btn_back = lv_btn_create(btn_cont);
  lv_obj_set_style_pad_all(btn_back, 2, 0);
  lv_obj_set_flex_grow(btn_back, 1);
  lv_obj_t *lbl_back = lv_label_create(btn_back);
  lv_label_set_text(lbl_back, "Back");
  lv_obj_center(lbl_back);
  lv_obj_add_event_cb(btn_back, cb_sched_back_btn, LV_EVENT_CLICKED, NULL);
  lv_group_add_obj(sched_group, btn_back);
}

static void sched_show_list(void) {
  lv_obj_clean(sched_root);

  sched_cont = lv_obj_create(sched_root);
  lv_obj_set_size(sched_cont, LV_PCT(100), LV_PCT(100));
  lv_obj_set_flex_flow(sched_cont, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_scrollbar_mode(sched_cont, LV_SCROLLBAR_MODE_OFF);
  lv_obj_set_style_pad_all(sched_cont, 5, 0);

  lv_obj_t *title = lv_label_create(sched_cont);
  lv_label_set_text(title, "Schedule");
  lv_obj_set_style_pad_hor(title, 5, 0);

  lv_obj_t *sched_list = lv_list_create(sched_cont);
  lv_obj_set_width(sched_list, LV_PCT(100));
  lv_obj_set_flex_grow(sched_list, 1);
  lv_obj_set_style_pad_all(sched_list, 5, 0);

  for (int i = 0; i < schedule_count; i++) {
    char label[16];
    snprintf(label, sizeof(label), "%02d:%02d - %dg", schedules[i].hour,
             schedules[i].min, schedules[i].portion);
    lv_obj_t *btn = lv_list_add_button(sched_list, NULL, label);
    lv_obj_set_style_pad_ver(btn, 2, 0);
    lv_obj_set_style_pad_hor(btn, 5, 0);
    lv_obj_add_event_cb(btn, cb_sched_edit_btn, LV_EVENT_CLICKED,
                        &schedules[i]);
    lv_group_add_obj(sched_group, btn);
  }

  lv_obj_t *btn_cont = lv_obj_create(sched_cont);
  lv_obj_set_size(btn_cont, LV_PCT(100), LV_SIZE_CONTENT);
  lv_obj_set_flex_flow(btn_cont, LV_FLEX_FLOW_ROW);
  lv_obj_set_style_pad_all(btn_cont, 5, 0);

  lv_obj_t *btn_new = lv_btn_create(btn_cont);
  lv_obj_set_style_pad_all(btn_new, 2, 0);
  lv_obj_set_flex_grow(btn_new, 1);
  lv_obj_t *lbl_new = lv_label_create(btn_new);
  lv_label_set_text(lbl_new, "New");
  lv_obj_center(lbl_new);
  lv_obj_add_event_cb(btn_new, cb_sched_new_btn, LV_EVENT_CLICKED, NULL);
  lv_group_add_obj(sched_group, btn_new);

  lv_obj_t *btn_back = lv_btn_create(btn_cont);
  lv_obj_set_style_pad_all(btn_back, 2, 0);
  lv_obj_set_flex_grow(btn_back, 1);
  lv_obj_t *lbl_back = lv_label_create(btn_back);
  lv_label_set_text(lbl_back, "Back");
  lv_obj_center(lbl_back);
  lv_obj_add_event_cb(btn_back, cb_sched_main_back_btn, LV_EVENT_CLICKED, NULL);
  lv_group_add_obj(sched_group, btn_back);
}

static int minutes_from_midnight(int hour, int min) { return hour * 60 + min; }

static int get_current_minutes(void) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  struct tm *now = localtime(&tv.tv_sec);
  return now->tm_hour * 60 + now->tm_min;
}

static void sched_timer_cb(void *arg) {
  int curr = get_current_minutes();
  ESP_LOGI(TAG, "Timer trigger: %02d:%02d", curr / 60, curr % 60);

  sched_trigger_action();

  sched_arm_next();
}

static void sched_arm_next(void) {
  if (sched_timer) {
    esp_timer_stop(sched_timer);
  }

  if (schedule_count == 0)
    return;

  int now_min  = get_current_minutes();
  int min_diff = 1440;
  int index    = -1;

  // Iteratively finding the next closest entry
  for (int i = 0; i < schedule_count; ++i) {
    int sched_min = minutes_from_midnight(schedules[i].hour, schedules[i].min);
    int diff      = sched_min - now_min;
    // Next day's entries
    if (diff <= 0)
      diff += 1440;
    if (diff <= min_diff) {
      min_diff = diff;
      index    = i;
    }
  }

  ESP_LOGI(TAG, "Next trigger: %02d:%02d in %02d hr %02d mins",
           schedules[index].hour, schedules[index].min, min_diff / 60,
           min_diff % 60);

  esp_timer_start_once(sched_timer, (int64_t)min_diff * 60 * 1000000);
}

esp_err_t sched_timer_init(void) {
  esp_timer_create_args_t timer_args = {
      .callback = sched_timer_cb, .arg = NULL, .name = "sched_timer"};
  esp_err_t ret = esp_timer_create(&timer_args, &sched_timer);
  if (ret == ESP_OK) {
    ESP_LOGI(TAG, "Schedule timer initialized");
    sched_arm_next();
  }

  return ret;
}

void sched_create(lv_obj_t *parent) {
  sched_root  = parent;
  sched_group = lv_group_create();
  lv_group_set_default(sched_group);
  if (hardware.encoder) {
    lv_indev_set_group(hardware.encoder, sched_group);
  }
  sched_show_list();
}

static void sched_trigger_action(void) {
  int food_level = get_distance();

  if (food_level < 0) {
    show_warning("Sensor Error", "Failed to read from ultrasonic sensor");
    return;
  }

  servo_dispense(10);
}

void show_warning(const char *title, const char *msg) {
  lv_obj_t *msgbox   = lv_msgbox_create(NULL);
  lv_obj_t *msgtitle = lv_msgbox_add_title(msgbox, title);
  lv_obj_set_style_pad_ver(msgtitle, 2, 0);
  lv_obj_set_style_pad_hor(msgtitle, 5, 0);
  lv_obj_t *text = lv_msgbox_add_text(msgbox, msg);
  lv_obj_set_style_pad_all(text, 5, 0);
  lv_obj_t *btn_close = lv_msgbox_add_close_button(msgbox);
  lv_obj_set_style_pad_all(btn_close, 0, 0);
  lv_obj_set_size(msgbox, LV_PCT(75), LV_PCT(75));
}
