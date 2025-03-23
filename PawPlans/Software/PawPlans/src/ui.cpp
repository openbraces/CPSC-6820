#include "M5DinMeter.h"

namespace ui {
int32_t encpos = 0;
void    init() {
  DinMeter.Display.setRotation(1);
  DinMeter.Display.setTextColor(GREEN);
  DinMeter.Display.setTextDatum(middle_center);
  DinMeter.Display.setTextFont(&fonts::Roboto_Thin_24);
  DinMeter.Display.setTextSize(2);

  encpos = DinMeter.Encoder.read();
}
void update() {
  int32_t currpos = DinMeter.Encoder.read();
  if (currpos != encpos) {
    DinMeter.Speaker.tone(8000, 20);
    DinMeter.Display.clear();
    encpos = currpos;
    Serial.println(currpos);
    DinMeter.Display.drawString(String(currpos),
                                DinMeter.Display.width() / 2,
                                DinMeter.Display.height() / 2);
  }
  if (DinMeter.BtnA.wasPressed()) {
    DinMeter.Encoder.readAndReset();
  }
  if (DinMeter.BtnA.pressedFor(5000)) {
    DinMeter.Encoder.write(100);
  }
}
} // namespace ui