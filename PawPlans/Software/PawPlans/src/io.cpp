#include "M5DinMeter.h"
#include "M5UnitWeightI2C.h"
#include "Unit_Sonic.h"

#include "io.hpp"

namespace io {
long            encoder_position = -999;
SONIC_I2C       distance_sensor;
M5UnitWeightI2C weight_sensor;

inline input_event read_encoder() {
  auto current_position = DinMeter.Encoder.read();
  auto button_state     = DinMeter.BtnA.wasPressed();
  auto event            = button_state                          ? SELECT
                          : current_position > encoder_position ? FORWARD
                          : current_position < encoder_position ? BACKWARD
                                                                : NIL;
  encoder_position      = current_position;

  return event;
}

inline void play_buzzer(float frequency, long duration) {
  DinMeter.Speaker.tone(frequency, duration);
}

inline void        distance_sensor_init() { distance_sensor.begin(); }
inline const float read_distance_sensor() {
  return distance_sensor.getDistance();
}

inline void weight_sensor_init() {
  while (!weight_sensor.begin(&Wire, M5.Ex_I2C.getSCL(), M5.Ex_I2C.getSDA(),
                              DEVICE_DEFAULT_ADDR)) {
    Serial.println("[EE] Cannot connect to weight sensor");
    delay(100);
  }
}
inline void        calibrate_weight_sensor() { weight_sensor.setOffset(); }
inline const float read_weight_sensor() { return weight_sensor.getWeight(); }
} // namespace io
