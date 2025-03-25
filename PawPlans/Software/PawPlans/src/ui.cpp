#include "M5DinMeter.h"

#include "io.hpp"
#include "menu.hpp"
#include "scheduling.hpp"
#include "ui.hpp"

namespace ui {
bool wifi_status   = false;
bool system_status = true;

menu::header   header;
menu::root     root;
menu::wifi     wifi;
menu::clock    clock;
menu::schedule schedule;

menutype current_menu = ROOT;

void render(menu::input_event event) {
  strcpy(render_string, "");
  header.render(event);
  switch (current_menu) {
  case ROOT:
    root.render(event);
    break;
  case WIFI:
    wifi.render(event);
    break;
  case CLOCK:
    clock.render(event);
    break;
  case SCHEDULE:
    schedule.render(event);
    break;
  }
}

void init() {
  DinMeter.Display.setRotation(1);
  DinMeter.Display.setTextColor(GREEN);
  DinMeter.Display.setTextDatum(middle_center);
  DinMeter.Display.setTextSize(2);
  DinMeter.Display.setFont(&fonts::Roboto_Thin_24);

  auto encpos = DinMeter.Encoder.read();
}

void update() {
  render(menu::input_event::SELECT);
  DinMeter.Display.clear();
  Serial.println(render_string);
  DinMeter.Display.drawString(render_string, 0, 0);
}

void format_time(scheduling::time time, char time_string[5]) {
  sprintf(time_string, "%02d:%02d", time.hours, time.minutes);
}

const auto max = [](int8_t x, int8_t y) { return x > y ? x : y; };
const auto min = [](int8_t x, int8_t y) { return x < y ? y : x; };
} // namespace ui