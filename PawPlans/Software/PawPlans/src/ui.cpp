#include "M5DinMeter.h"
#include <string>
#include <cstring>

namespace ui {
enum Menus {
  MAIN,
  WIFI,
  CLOCK,
  SCHEDULE
};

char string_output[256]; // I just set it to 256 chars because that should be enough

bool wifi_enabled = false;
int current_hour = 0;
int current_minute = 0;
int previous_set_hour = 0;
int previous_set_minute = 0;
int next_set_hour = 0;
int next_set_minute = 0;
bool overall_status = true;

Menus current_menu = MAIN; // (Default MAIN)
Menus current_selection = WIFI; // This is for the homescreen where the user would select which menu it wants to select (Default WIFI)

void    init() {
  DinMeter.Display.setRotation(1);
  DinMeter.Display.setTextColor(GREEN);
  DinMeter.Display.setTextDatum(middle_center);
  DinMeter.Display.setTextFont(&fonts::Roboto_Thin_24);
  DinMeter.Display.setTextSize(2);

  encpos = DinMeter.Encoder.read();
}

std::string format_time_string(int hour, int minute) {
  return std::format("{:02d}:{:02d}", hour, minute);
}

std::string format_wifi_string(bool wifi_enabled) {
  return wifi_enabled ? "ON" : "OFF";
}

std::string update_header_string() {
  std::string status_str = format_overall_status(overall_status);
  std::string current_time_str = format_time_string(current_hour, current_minute);
  std::string previous_time_str = format_time_string(previous_hour, previous_minute);
  std::string next_time_str = format_time_string(next_hour, next_minute);
  std::string wifi_str = format_wifi_string(wifi_enabled);

  std::string header = std::format("WiFi {} T {} PRE {} NXT {} {}", 
                                   wifi_str, 
                                   current_time_str, 
                                   previous_time_str, 
                                   next_time_str, 
                                   status_str);

  return header;
}

std::string update_main_menu_string(Menus current_selection) {
  // Adds a > if the the line is the current_selection
  std::string wifi_line = (current_selection == WIFI) ? "> WiFi\n" : "WiFi\n";
  std::string clock_line = (current_selection == CLOCK) ? "> Clock\n" : "Clock\n";
  std::string schedule_line = (current_selection == SCHEDULE) ? "> Schedule\n" : "Schedule\n";

  std::string menu = std::format("PawPlans\n{}{}{}", wifi_line, clock_line, schedule_line);

  return menu;
}

std::string update_output_string() {
  std::string header_string = update_header_string();
  std::string main_menu_string = update_main_menu_string();
  update_header_string(header_string);
  update_main_menu_string(current_selection);

  if (current_menu == MAIN) {
    return std::format("{}\n{}", header_string, main_menu_string);
  }
}

void update() {
  DinMeter.Speaker.tone(8000, 20);
  DinMeter.Display.clear();
  Serial.println(string_output); // For debugging
  DinMeter.Display.drawString(string_output,
                              0,
                              0); // x = 0, y = 0 should be top left of screen

  if (DinMeter.BtnA.wasPressed()) {
    DinMeter.Encoder.readAndReset();
  }
  if (DinMeter.BtnA.pressedFor(5000)) {
    DinMeter.Encoder.write(100);
  }
  DinMeter.Display.
}
} // namespace ui