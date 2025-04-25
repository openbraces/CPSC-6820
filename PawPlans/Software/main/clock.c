#include "clock.h"
#include "hardware.h"
#include "ui.h"

#define TAG "clock"

static void back_btn_cb(lv_event_t *e) { ui_back_main(); }

static void set_time_cb(lv_event_t *e) {
  lv_obj_t *cont = lv_obj_get_parent(lv_obj_get_parent(lv_event_get_target(e)));
  lv_obj_t *rollers_cont = lv_obj_get_child(cont, 1);

  lv_obj_t *hour_roller = lv_obj_get_child(rollers_cont, 0);
  lv_obj_t *min_roller  = lv_obj_get_child(rollers_cont, 2);

  uint16_t hour   = lv_roller_get_selected(hour_roller);
  uint16_t minute = lv_roller_get_selected(min_roller);

  struct timeval  tv;
  struct timezone tz;
  gettimeofday(&tv, &tz);

  struct tm *tm = localtime(&tv.tv_sec);
  tm->tm_hour   = hour;
  tm->tm_min    = minute;

  time_t new_time = mktime(tm);
  tv.tv_sec       = new_time;

  if (settimeofday(&tv, &tz) == -1) {
    ESP_LOGE(TAG, "Failed to set time");
  } else {
    ESP_LOGI(TAG, "System clock set to %02d:%02d", hour, minute);
  }

  ui_back_main();
}

void clock_create(lv_obj_t *parent) {
  lv_obj_t *root = lv_obj_create(parent);
  lv_obj_set_size(root, lv_pct(100), lv_pct(100));
  lv_obj_set_flex_flow(root, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_style_pad_all(root, 5, 0);

  lv_obj_t *title = lv_label_create(root);
  lv_label_set_text(title, "Set Time");
  lv_obj_set_style_pad_hor(title, 5, 0);

  lv_obj_t *rollers_cont = lv_obj_create(root);
  lv_obj_set_width(rollers_cont, lv_pct(100));
  lv_obj_set_flex_grow(rollers_cont, 1);
  lv_obj_set_flex_flow(rollers_cont, LV_FLEX_FLOW_ROW);
  lv_obj_set_style_pad_all(rollers_cont, 5, 0);
  lv_obj_set_style_pad_gap(rollers_cont, 10, 0);
  lv_obj_set_flex_align(rollers_cont, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

  struct timeval tv;
  gettimeofday(&tv, NULL);
  struct tm *tm = localtime(&tv.tv_sec);

  lv_obj_t *hour_roller = lv_roller_create(rollers_cont);
  lv_roller_set_options(hour_roller, CLOCK_HOUR_OPTIONS, LV_ROLLER_MODE_NORMAL);
  lv_obj_set_flex_grow(hour_roller, 1);
  lv_roller_set_selected(hour_roller, tm->tm_hour, LV_ANIM_ON);

  // Colon separator
  lv_obj_t *colon = lv_label_create(rollers_cont);
  lv_label_set_text(colon, ":");

  lv_obj_t *min_roller = lv_roller_create(rollers_cont);
  lv_roller_set_options(min_roller, CLOCK_MINUTE_OPTIONS,
                        LV_ROLLER_MODE_NORMAL);
  lv_obj_set_flex_grow(min_roller, 1);
  lv_roller_set_selected(min_roller, tm->tm_min, LV_ANIM_ON);

  // TODO: Add a third roller for seconds

  lv_obj_t *btn_cont = lv_obj_create(root);
  lv_obj_set_size(btn_cont, lv_pct(100), LV_SIZE_CONTENT);
  lv_obj_set_flex_flow(btn_cont, LV_FLEX_FLOW_ROW);
  lv_obj_set_style_pad_all(btn_cont, 5, 0);

  lv_obj_t *set_btn = lv_btn_create(btn_cont);
  lv_obj_set_style_pad_all(set_btn, 2, 0);
  lv_obj_set_flex_grow(set_btn, 1);
  lv_obj_t *set_label = lv_label_create(set_btn);
  lv_label_set_text(set_label, "Set");
  lv_obj_center(set_label);
  lv_obj_add_event_cb(set_btn, set_time_cb, LV_EVENT_CLICKED, NULL);

  lv_obj_t *back_btn = lv_btn_create(btn_cont);
  lv_obj_set_style_pad_all(back_btn, 2, 0);
  lv_obj_set_flex_grow(back_btn, 1);
  lv_obj_t *back_label = lv_label_create(back_btn);
  lv_label_set_text(back_label, "Back");
  lv_obj_center(back_label);
  lv_obj_add_event_cb(back_btn, back_btn_cb, LV_EVENT_CLICKED, NULL);

  lv_group_t *group = lv_group_create();
  lv_group_set_default(group);
  lv_group_add_obj(group, hour_roller);
  lv_group_add_obj(group, min_roller);
  lv_group_add_obj(group, set_btn);
  lv_group_add_obj(group, back_btn);

  if (hardware.encoder) {
    lv_indev_set_group(hardware.encoder, group);
  }
}
