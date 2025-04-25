#include "debug.h"
#include "hardware.h"
#include "io.h"
#include "ui.h"

static void refresh_btn_cb(lv_event_t *e) { ui_load_debug(); }

static void back_btn_cb(lv_event_t *e) { ui_back_main(); }

static void servo_btn_cb(lv_event_t *e) { servo_dispense(10); }

static void buzzer_cb(lv_event_t *e) { buzzer_beep(); }

void debug_create(lv_obj_t *parent) {
  lv_obj_t *root = lv_obj_create(parent);
  lv_obj_set_size(root, lv_pct(100), lv_pct(100));
  lv_obj_set_flex_flow(root, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_style_pad_all(root, 5, 0);

  lv_obj_t *title = lv_label_create(root);
  lv_label_set_text(title, "PawPlans - Sensors");
  lv_obj_set_style_pad_hor(title, 5, 0);

  lv_obj_t *list = lv_list_create(root);
  lv_obj_set_size(list, lv_pct(100), lv_pct(100));
  lv_obj_set_flex_grow(list, 1);
  lv_obj_set_style_pad_all(list, 5, 0);

  char text[20];
  snprintf(text, sizeof(text), "Weight: %4.2f g", get_weight());
  lv_obj_t *btn_weight = lv_list_add_button(list, NULL, text);
  lv_obj_set_style_pad_ver(btn_weight, 2, 0);
  lv_obj_set_style_pad_hor(btn_weight, 5, 0);

  snprintf(text, sizeof(text), "Distance: %4.2f mm", get_distance());
  lv_obj_t *btn_ultrasonic = lv_list_add_button(list, NULL, text);
  lv_obj_set_style_pad_ver(btn_ultrasonic, 2, 0);
  lv_obj_set_style_pad_hor(btn_ultrasonic, 5, 0);

  snprintf(text, sizeof(text), "Servo: %4.2f deg", curr_servo_angle);
  lv_obj_t *btn_servo = lv_list_add_button(list, NULL, text);
  lv_obj_set_style_pad_ver(btn_servo, 2, 0);
  lv_obj_set_style_pad_hor(btn_servo, 5, 0);
  lv_obj_add_event_cb(btn_servo, servo_btn_cb, LV_EVENT_CLICKED, NULL);

  snprintf(text, sizeof(text), "Buzzer");
  lv_obj_t *btn_buzzer = lv_list_add_button(list, NULL, text);
  lv_obj_set_style_pad_ver(btn_buzzer, 2, 0);
  lv_obj_set_style_pad_hor(btn_buzzer, 5, 0);
  lv_obj_add_event_cb(btn_buzzer, buzzer_cb, LV_EVENT_CLICKED, NULL);

  lv_obj_t *btn_cont = lv_obj_create(root);
  lv_obj_set_size(btn_cont, lv_pct(100), LV_SIZE_CONTENT);
  lv_obj_set_flex_flow(btn_cont, LV_FLEX_FLOW_ROW);
  lv_obj_set_style_pad_all(btn_cont, 5, 0);

  lv_obj_t *btn_refresh = lv_btn_create(btn_cont);
  lv_obj_set_style_pad_all(btn_refresh, 2, 0);
  lv_obj_set_flex_grow(btn_refresh, 1);
  lv_obj_t *label_refresh = lv_label_create(btn_refresh);
  lv_label_set_text(label_refresh, "Refresh");
  lv_obj_center(label_refresh);
  lv_obj_add_event_cb(btn_refresh, refresh_btn_cb, LV_EVENT_CLICKED, NULL);

  lv_obj_t *btn_back = lv_btn_create(btn_cont);
  lv_obj_set_style_pad_all(btn_back, 2, 0);
  lv_obj_set_flex_grow(btn_back, 1);
  lv_obj_t *label_back = lv_label_create(btn_back);
  lv_label_set_text(label_back, "Back");
  lv_obj_center(label_back);
  lv_obj_add_event_cb(btn_back, back_btn_cb, LV_EVENT_CLICKED, NULL);

  lv_group_t *group = lv_group_create();
  lv_group_set_default(group);

  lv_group_add_obj(group, btn_weight);
  lv_group_add_obj(group, btn_ultrasonic);
  lv_group_add_obj(group, btn_servo);
  lv_group_add_obj(group, btn_buzzer);
  lv_group_add_obj(group, btn_refresh);
  lv_group_add_obj(group, btn_back);

  if (hardware.encoder) {
    lv_indev_set_group(hardware.encoder, group);
  }
}
