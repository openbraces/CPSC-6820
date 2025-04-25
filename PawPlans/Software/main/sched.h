#pragma once

#include "esp_lvgl_port.h"

void      sched_create(lv_obj_t *parent);
esp_err_t sched_timer_init(void);
