#include "hardware.h"
#include "esp_log.h"

#include "io.h"

#define TAG "hw_init"

hw_handle_t hardware;

esp_err_t hw_init(void) {
  // Backlight configuration
  gpio_config_t bk_gpio_cfg = {.pin_bit_mask = 1ULL << DISPLAY_BL,
                               .mode         = GPIO_MODE_OUTPUT,
                               .pull_up_en   = GPIO_PULLUP_DISABLE,
                               .pull_down_en = GPIO_PULLDOWN_DISABLE,
                               .intr_type    = GPIO_INTR_DISABLE};
  ESP_RETURN_ON_ERROR(gpio_config(&bk_gpio_cfg), TAG,
                      "Backlight GPIO config failed");
  gpio_set_level(DISPLAY_BL, 1);

  // SPI bus configuration
  spi_bus_config_t spi_cfg = {.sclk_io_num     = DISPLAY_SCK,
                              .mosi_io_num     = DISPLAY_MOSI,
                              .miso_io_num     = GPIO_NUM_NC,
                              .quadwp_io_num   = GPIO_NUM_NC,
                              .quadhd_io_num   = GPIO_NUM_NC,
                              .max_transfer_sz = DISPLAY_CROP_H_RES *
                                                 DISPLAY_DRAW_BUF_HEIGHT *
                                                 sizeof(uint16_t),
                              .flags      = SPICOMMON_BUSFLAG_MASTER,
                              .intr_flags = 0};
  ESP_RETURN_ON_ERROR(
      spi_bus_initialize(DISPLAY_SPI_HOST, &spi_cfg, SPI_DMA_CH_AUTO), TAG,
      "SPI init failed");

  // LCD IO configuration
  esp_lcd_panel_io_spi_config_t io_cfg = {.dc_gpio_num    = DISPLAY_RS,
                                          .cs_gpio_num    = DISPLAY_CS,
                                          .pclk_hz        = DISPLAY_PCLK_HZ,
                                          .lcd_cmd_bits   = DISPLAY_CMD_BITS,
                                          .lcd_param_bits = DISPLAY_PARAM_BITS,
                                          .spi_mode       = 0,
                                          .trans_queue_depth = 10};
  ESP_RETURN_ON_ERROR(
      esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)DISPLAY_SPI_HOST,
                               &io_cfg, &hardware.io),
      TAG, "LCD IO init failed");

  // LCD panel configuration
  esp_lcd_panel_dev_config_t panel_cfg = {.reset_gpio_num = DISPLAY_RST,
                                          .color_space    = DISPLAY_COLORSPACE,
                                          .bits_per_pixel = DISPLAY_BIT_DEPTH};
  ESP_RETURN_ON_ERROR(
      esp_lcd_new_panel_st7789(hardware.io, &panel_cfg, &hardware.panel), TAG,
      "LCD panel init failed");

  // Initialize panel
  esp_lcd_panel_reset(hardware.panel);
  esp_lcd_panel_init(hardware.panel);
  esp_lcd_panel_disp_on_off(hardware.panel, true);

  // LVGL port configuration
  const lvgl_port_cfg_t lvgl_cfg = {.task_priority     = 4,
                                    .task_stack        = 8192,
                                    .task_affinity     = -1,
                                    .task_max_sleep_ms = 500,
                                    .timer_period_ms   = 5};
  ESP_RETURN_ON_ERROR(lvgl_port_init(&lvgl_cfg), TAG, "LVGL port init failed");

  // LVGL display configuration
  const lvgl_port_display_cfg_t disp_cfg = {
      .io_handle     = hardware.io,
      .panel_handle  = hardware.panel,
      .buffer_size   = DISPLAY_CROP_H_RES * DISPLAY_DRAW_BUF_HEIGHT,
      .double_buffer = true,
      .hres          = DISPLAY_PHY_H_RES,
      .vres          = DISPLAY_PHY_V_RES,
      .rotation      = {.swap_xy = true, .mirror_x = true, .mirror_y = false},
      .flags         = {.buff_dma = true, .swap_bytes = true}
  };
  hardware.disp = lvgl_port_add_disp(&disp_cfg);
  lv_display_set_resolution(hardware.disp, DISPLAY_CROP_H_RES,
                            DISPLAY_CROP_V_RES);
  lv_display_set_offset(hardware.disp, DISPLAY_H_OFFSET, DISPLAY_V_OFFSET);

  // Encoder button configuration
  const button_config_t      btn_cfg;
  const button_gpio_config_t btn_gpio_cfg = {
      .gpio_num     = ENCODER_BTN,
      .active_level = 0,
  };
  ESP_RETURN_ON_ERROR(
      iot_button_new_gpio_device(&btn_cfg, &btn_gpio_cfg, &hardware.btn), TAG,
      "Encoder button init failed");

  // Encoder configuration
  const knob_config_t encoder_cfg = {
      .default_direction = 0,
      .gpio_encoder_a    = ENCODER_A,
      .gpio_encoder_b    = ENCODER_B,
  };
  const lvgl_port_encoder_cfg_t lv_encoder_cfg = {
      .disp          = hardware.disp,
      .encoder_a_b   = &encoder_cfg,
      .encoder_enter = hardware.btn,
  };
  hardware.encoder = lvgl_port_add_encoder(&lv_encoder_cfg);

  lv_indev_set_group(hardware.encoder, lv_group_get_default());

  io_init();

  ESP_LOGI(TAG, "Hardware initialized");
  return ESP_OK;
}

uint8_t hw_get_brightness(void) { return gpio_get_level(DISPLAY_BL) * 100; }

void hw_set_brightness(uint8_t brightness) {
  if (brightness > 100)
    brightness = 100;
  gpio_set_level(DISPLAY_BL, brightness > 0 ? 1 : 0);
}
