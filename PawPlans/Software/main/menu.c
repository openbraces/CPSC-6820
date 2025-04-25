#include "menu.h"
#include "hardware.h"
#include "ui.h"
#include <string.h>

#define MENU_ENTRIES 4

static void btn_cb(lv_event_t *e) {
  lv_obj_t   *btn = lv_event_get_target(e);
  const char *txt = lv_label_get_text(lv_obj_get_child(btn, 0));

  if (strcmp(txt, "WiFi") == 0) {
    // TODO: WiFi menu
  } else if (strcmp(txt, "Clock") == 0) {
    ui_load_clock();
  } else if (strcmp(txt, "Schedule") == 0) {
    ui_load_sched();
  } else if (strcmp(txt, "Debug") == 0) {
    ui_load_debug();
  }
}

void menu_create(lv_obj_t *parent) {
  lv_obj_t *root = lv_obj_create(parent);
  lv_obj_set_size(root, lv_pct(100), lv_pct(100));
  lv_obj_set_flex_flow(root, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_style_pad_all(root, 5, 0);

  lv_obj_t *title = lv_label_create(root);
  lv_label_set_text(title, "PawPlans - Menu");
  lv_obj_set_style_pad_hor(title, 5, 0);

  lv_obj_t *list = lv_list_create(root);
  lv_obj_set_size(list, lv_pct(100), lv_pct(100));
  lv_obj_set_flex_grow(list, 1);
  lv_obj_set_style_pad_all(list, 5, 0);

  const char *items[MENU_ENTRIES] = {"WiFi", "Clock", "Schedule", "Debug"};
  lv_obj_t   *btns[MENU_ENTRIES];

  for (int i = 0; i < MENU_ENTRIES; i++) {
    btns[i] = lv_list_add_btn(list, NULL, items[i]);
    lv_obj_add_event_cb(btns[i], btn_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_pad_ver(btns[i], 2, 0);
    lv_obj_set_style_pad_hor(btns[i], 5, 0);
  }

  lv_group_t *group = lv_group_create();
  lv_group_set_default(group);

  for (int i = 0; i < MENU_ENTRIES; i++) {
    lv_group_add_obj(group, btns[i]);
  }

  if (hardware.encoder) {
    lv_indev_set_group(hardware.encoder, group);
  }
}
