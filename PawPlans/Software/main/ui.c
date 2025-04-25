#include "ui.h"
#include "clock.h"
#include "debug.h"
#include "menu.h"
#include "sched.h"

#define TAG "ui"

static lv_obj_t *scr;

void ui_init(void) {
  scr = lv_screen_active();
  ui_load_main();
  ESP_LOGI(TAG, "UI initialized");
}

void ui_load_main(void) {
  lv_obj_clean(scr);
  menu_create(scr);
}

void ui_load_clock(void) {
  lv_obj_clean(scr);
  clock_create(scr);
}

void ui_load_sched(void) {
  lv_obj_clean(scr);
  sched_create(scr);
}

void ui_load_debug(void) {
  lv_obj_clean(scr);
  debug_create(scr);
}

void ui_back_main(void) { ui_load_main(); }
