#pragma once

#include <button_gpio.h>
#include <driver/gpio.h>
#include <driver/i2c.h>
#include <driver/i2c_master.h>
#include <driver/mcpwm_prelude.h>
#include <driver/spi_master.h>
#include <esp_check.h>
#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_ops.h>
#include <esp_lcd_panel_rgb.h>
#include <esp_lcd_panel_st7789.h>
#include <esp_lcd_types.h>
#include <esp_log.h>
#include <esp_lvgl_port.h>
#include <lvgl.h>
#include <stdint.h>

// Hardware pin definitions
// StampS3
// Port B: General GPIO
#define PORT_B_IN               (1)
#define PORT_B_OUT              (2)
// Port A: I2C communication
#define PORT_A_SDA              (13)
#define PORT_A_SCL              (15)
// Built-in RGB LED
#define RGB_LED                 (21)
#define POWER_HOLD              (46)

// Buzzer
#define BUZZER_PIN              (3)

// Encoder RTC8563
#define ENCODER_SDA             (11)
#define ENCODER_SCL             (12)
#define ENCODER_B               (40)
#define ENCODER_A               (41)
#define ENCODER_BTN             (42)

// LCD ST7789V2
#define DISPLAY_RS              (4)
#define DISPLAY_MOSI            (5)
#define DISPLAY_SCK             (6)
#define DISPLAY_CS              (7)
#define DISPLAY_RST             (8)
#define DISPLAY_BL              (9)
#define DISPLAY_PHY_H_RES       (320) // Physical width
#define DISPLAY_PHY_V_RES       (240) // Physical height
#define DISPLAY_CROP_H_RES      (240) // Cropped width
#define DISPLAY_CROP_V_RES      (135) // Cropped height
#define DISPLAY_H_OFFSET        ((DISPLAY_PHY_H_RES - DISPLAY_CROP_H_RES) / 2)
#define DISPLAY_V_OFFSET        ((DISPLAY_PHY_V_RES - DISPLAY_CROP_V_RES) / 2)
#define DISPLAY_DRAW_BUF_HEIGHT (50)
#define DISPLAY_COLORSPACE      (ESP_LCD_COLOR_SPACE_BGR)
#define DISPLAY_BIT_DEPTH       (16)
#define DISPLAY_SPI_HOST        (SPI2_HOST)
#define DISPLAY_PCLK_HZ         (40 * 1000 * 1000)
#define DISPLAY_CMD_BITS        (8)
#define DISPLAY_PARAM_BITS      (8)

// I2C addresses
#define I2C_ADDR_WEIGHT         (0x26)
#define I2C_ADDR_ULTRASONIC     (0x57)

// Servo control (PWM)
#define SERVO_MIN_PULSE         (500)  // in us
#define SERVO_MAX_PULSE         (2500) // in us
#define SERVO_ANGLE_RANGE       (180)
#define SERVO_TIMER_RES         (1000000) // in Hz
#define SERVO_TIMER_PERIOD      (20000)   // ticks = 20 ms, period = 50 Hz
#define SERVO_REST_ANGLE        (0)
#define SERVO_DISPENSE_ANGLE    (90)
#define SERVO_MIN_DELAY         (200)
#define SERVO_MAX_DELAY         (2000)

typedef struct {
  lv_display_t             *disp;
  lv_indev_t               *encoder;
  esp_lcd_panel_handle_t    panel;
  esp_lcd_panel_io_handle_t io;
  button_handle_t           btn;
} hw_handle_t;

extern hw_handle_t hardware;

// Hardware functions
esp_err_t hw_init(void);
uint8_t   hw_get_brightness(void);
void      hw_set_brightness(uint8_t brightness);
