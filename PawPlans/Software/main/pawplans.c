#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "hardware.h"
#include "sched.h"
#include "ui.h"

static const char *TAG = "main_task";

void app_main(void) {
  esp_err_t ret = hw_init();
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Hardware initialization failed: %s", esp_err_to_name(ret));
    return;
  }
  hw_set_brightness(80);
  ui_init();
  ESP_ERROR_CHECK(sched_timer_init());
  ESP_LOGI(TAG, "Application started");
}
