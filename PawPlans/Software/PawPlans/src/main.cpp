#include "M5DinMeter.h"

void setup() {
  auto cfg = M5.config();
  DinMeter.begin(cfg, true);
}

void loop() { DinMeter.update(); }
