#include "io.h"
#include "hardware.h"

#define TAG "io"

static i2c_master_bus_handle_t i2c_bus          = NULL;
static i2c_master_dev_handle_t weight_dev       = NULL;
static i2c_master_dev_handle_t ultrasonic_dev   = NULL;
static mcpwm_cmpr_handle_t     servo_comparator = NULL;
static mcpwm_oper_handle_t     servo_operator   = NULL;
float                          curr_servo_angle = 0;

void io_init(void) {
  // I2C master bus init
  i2c_master_bus_config_t i2c_config = {
      .clk_source                   = I2C_CLK_SRC_DEFAULT,
      .i2c_port                     = I2C_NUM_0,
      .scl_io_num                   = PORT_A_SCL,
      .sda_io_num                   = PORT_A_SDA,
      .glitch_ignore_cnt            = 7,
      .flags.enable_internal_pullup = true,
  };
  ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_config, &i2c_bus));

  // Weight sensor
  i2c_device_config_t weight_cfg = {
      .dev_addr_length = I2C_ADDR_BIT_LEN_7,
      .device_address  = I2C_ADDR_WEIGHT,
      .scl_speed_hz    = 100000,
  };
  ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_bus, &weight_cfg, &weight_dev));

  // Ultrasonic sensor
  i2c_device_config_t ultrasonic_cfg = {
      .dev_addr_length = I2C_ADDR_BIT_LEN_7,
      .device_address  = I2C_ADDR_ULTRASONIC,
      .scl_speed_hz    = 100000,
  };
  ESP_ERROR_CHECK(
      i2c_master_bus_add_device(i2c_bus, &ultrasonic_cfg, &ultrasonic_dev));

  // Servo configuration
  mcpwm_timer_handle_t timer        = NULL;
  mcpwm_timer_config_t timer_config = {
      .group_id      = 0,
      .clk_src       = MCPWM_TIMER_CLK_SRC_DEFAULT,
      .resolution_hz = SERVO_TIMER_RES,
      .period_ticks  = SERVO_TIMER_PERIOD,
      .count_mode    = MCPWM_TIMER_COUNT_MODE_UP,
  };
  ESP_ERROR_CHECK(mcpwm_new_timer(&timer_config, &timer));

  mcpwm_operator_config_t op_config = {.group_id = 0};
  ESP_ERROR_CHECK(mcpwm_new_operator(&op_config, &servo_operator));
  ESP_ERROR_CHECK(mcpwm_operator_connect_timer(servo_operator, timer));

  mcpwm_comparator_config_t cmp_config = {.flags.update_cmp_on_tez = true};
  ESP_ERROR_CHECK(
      mcpwm_new_comparator(servo_operator, &cmp_config, &servo_comparator));

  mcpwm_gen_handle_t       generator  = NULL;
  mcpwm_generator_config_t gen_config = {.gen_gpio_num = PORT_B_OUT};
  ESP_ERROR_CHECK(mcpwm_new_generator(servo_operator, &gen_config, &generator));
  ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(
      generator, MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP,
                                              MCPWM_TIMER_EVENT_EMPTY,
                                              MCPWM_GEN_ACTION_HIGH)));
  ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(
      generator,
      MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, servo_comparator,
                                     MCPWM_GEN_ACTION_LOW)));

  ESP_ERROR_CHECK(mcpwm_timer_enable(timer));
  ESP_ERROR_CHECK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));
}

float get_distance(void) {
  uint8_t cmd = 0x01;
  uint8_t data[3]; // 24-bit data output from sensor (from M5Stack's code)

  esp_err_t err = i2c_master_transmit(ultrasonic_dev, &cmd, 1, -1);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Ultrasonic TX failed: %s", esp_err_to_name(err));
    return -1.0f;
  }

  // TODO: Test if this is needed
  vTaskDelay(pdMS_TO_TICKS(120)); // to match delay() from M5Stack's code

  err = i2c_master_receive(ultrasonic_dev, data, 3, -1);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Ultrasonic RX failed: %s", esp_err_to_name(err));
    return -1.0f;
  }

  uint32_t raw = ((uint32_t)data[0] << 16) | ((uint32_t)data[1] << 8) | data[2];
  float    distance = raw / 1000.0f;

  if (distance > 450.0f) {
    return 450.0f; // Clamping to max rated distance (from M5Stack's data)
  }
  return distance;
}

float get_weight(void) {
  uint8_t cmd = 0x11;
  uint8_t data[4];

  esp_err_t err = i2c_master_transmit(weight_dev, &cmd, 1, -1);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Weight TX failed: %s", esp_err_to_name(err));
    return -1.0f;
  }

  vTaskDelay(pdMS_TO_TICKS(120));

  err = i2c_master_receive(weight_dev, data, 4, -1);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Weight RX failed: %s", esp_err_to_name(err));
    return -1.0f;
  }

  int32_t raw = ((int32_t)(int8_t)data[0] << 24) | ((int32_t)data[1] << 16) |
                ((int32_t)data[2] << 8) | ((int32_t)data[3]);
  ESP_LOGI(TAG, "Raw weight bytes: %02X %02X %02X %02X", data[0], data[1],
           data[2], data[3]);
  ESP_LOGI(TAG, "Parsed int: %ld", raw);

  return (float)raw / 1000.0f;
}

void set_servo_angle(float angle) {
  if (angle < 0.0f)
    angle = 0.0f;
  if (angle > SERVO_ANGLE_RANGE)
    angle = SERVO_ANGLE_RANGE;

  float pulse = SERVO_MIN_PULSE + (angle / SERVO_ANGLE_RANGE) *
                                      (SERVO_MAX_PULSE - SERVO_MIN_PULSE);

  ESP_ERROR_CHECK(
      mcpwm_comparator_set_compare_value(servo_comparator, (uint32_t)pulse));

  curr_servo_angle = angle;
}

void servo_dispense(int portion) {
  if (portion <= 0)
    return;
  if (portion > 1000)
    portion = 1000;

  int delay =
      SERVO_MIN_DELAY + ((SERVO_MAX_DELAY - SERVO_MIN_DELAY) * portion) / 1000;

  set_servo_angle(SERVO_DISPENSE_ANGLE);
  vTaskDelay(pdMS_TO_TICKS(delay));
  set_servo_angle(SERVO_REST_ANGLE);
}

void buzzer_beep(void) {
  gpio_set_direction(BUZZER_PIN, GPIO_MODE_OUTPUT);
  gpio_set_level(BUZZER_PIN, 1);
  vTaskDelay(pdMS_TO_TICKS(100)); // 100 ms beep
  gpio_set_level(BUZZER_PIN, 0);
}
