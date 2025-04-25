#pragma once

#include "esp_log.h"
#include "esp_lvgl_port.h"

void ui_init(void);
void ui_load_main(void);
void ui_load_clock(void);
void ui_load_sched(void);
void ui_back_main(void);
void ui_load_debug(void);
