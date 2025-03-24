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

enum All_Selections {
  MAIN,
  WIFI,
  CLOCK,
  SCHEDULE,
  HOURS,
  MINUTES,
  SET,
  CANCEL,
  ADD,
  DELETE,
  VIEW,
}

char string_output[256]; // I just set it to 256 chars because that should be enough

bool wifi_enabled = false;
int current_hour = 0;
int current_minute = 0;
int previous_set_hour = 0;
int previous_set_minute = 0;
int next_set_hour = 0;
int next_set_minute = 0;
bool overall_status = true;

// For adding a time to the schedule
int add_time_hour = 0;
int add_time_minute = 0;

// For dealing with schedule times
int current_time_selection = 0; // current time selected in schedule 
int times[][2]; // all times in schedule
int num_times = 0; // number of times in the schedule

Menus current_menu = MAIN; // (Default MAIN)
All_Selections current_selection = WIFI; // This will represent any selection the user makes

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

std::string update_main_menu_string(All_Selections current_selection) {
  // Adds a > if the the line is the current_selection
  std::string wifi_line = (current_selection == WIFI) ? "> WiFi\n" : "WiFi\n";
  std::string clock_line = (current_selection == CLOCK) ? "> Clock\n" : "Clock\n";
  std::string schedule_line = (current_selection == SCHEDULE) ? "> Schedule\n" : "Schedule\n";

  std::string menu = std::format("PawPlans\n{}{}{}", wifi_line, clock_line, schedule_line);

  return menu;
}

std::string update_wifi_menu_string(All_Selections current_selection) {
  std::string wifi_placeholder = "Wifi\nPlaceholder for wifi menu";
  return wifi_placeholder;
}

std::string update_clock_menu_string(All_Selections current_selection) {
  std::string current_time_str = format_time_string(current_hour, current_minute);
  std::string time_line;

  if (current_selection == HOURS) {
    time_line = std::format("> [{}]:{}\n", current_time_str.substr(0, 2), current_time_str.substr(3, 2));
  }
  else if (current_selection == MINUTES) {
    time_line = std::format("> {}:[{}]\n", current_time_str.substr(0, 2), current_time_str.substr(3, 2));
  }
  else {
    time_line = std::format("{}\n", current_time_str);
  }

  std::string set_line = (current_selection == SET) ? "> Set\n" : "Set\n";
  std::string cancel_line = (current_selection == CANCEL) ? "> Cancel\n" : "Cancel\n";

  std::string menu = std::format("Clock\n{}{}{}", time_line, set_line, cancel_line);
  return menu;
}

std::string update_schedule_menu_string(All_Selections current_selection) {
  std::string add_line = (current_selection == ADD) ? "> Add\n" : "Add\n";
  std::string delete_line = (current_selection == DELETE) ? "> Delete\n" : "Delete\n";
  std::string view_line = (current_selection == VIEW) ? "> View\n" : "View\n";
  std::string cancel_line = (current_selection == CANCEL) ? "> Cancel\n" : "Cancel\n";

  std::string menu = std::format("Schedule\n{}{}{}", add_line, delete_line, view_line, cancel_line);
  return menu;
}

std::string update_add_time_menu_string(All_Selections current_selection, int new_hour, int new_minute) {
  std::string new_time_str = format_time_string(new_hour, new_minute);
  std::string time_line;

  if (current_selection == HOURS) {
    time_line = std::format("> [{}]:{}\n", new_time_str.substr(0, 2), new_time_str.substr(3, 2));
  }
  else if (current_selection == MINUTES) {
    time_line = std::format("> {}:[{}]\n", new_time_str.substr(0, 2), new_time_str.substr(3, 2));
  }
  else {
    time_line = std::format("{}\n", new_time_str);
  }

  std::string set_line = (current_selection == SET) ? "> Set\n" : "Set\n";
  std::string cancel_line = (current_selection == CANCEL) ? "> Cancel\n" : "Cancel\n";

  std::string menu = std::format("Add time\n{}{}{}", time_line, set_line, cancel_line);
  return menu;
}

// This takes an int as current_selection since it is the index of the time
std::string update_delete_time_menu_string(int current_time_selection, int times[][2], int num_times) {
  std::string menu = "Delete time\n";
  for (int i = 0; i < num_times; i++) {
      std::string time_str = format_time_string(times[i][0], times[i][1]);
      if (current_selection == i) {
          menu += std::format("> {}\n", time_str);
      } else {
          menu += std::format("{}\n", time_str);
      }
  }
  return menu;
}

std::string update_view_times_menu_string(int times[][2], int num_times) {
  std::string menu = "View times\n";
  for (int i = 0; i < num_times; i++) {
      std::string time_str = format_time_string(times[i][0], times[i][1]);
      menu += std::format("{}\n", time_str);
  }
  return menu;
}

std::string update_output_string() {
  std::string header_string = update_header_string();
  update_header_string(header_string);

  switch(current_menu) {
    case MAIN:
      std::string main_menu_string = update_main_menu_string(current_selection);
      update_main_menu_string(current_selection);
      return std::format("{}\n{}", header_string, main_menu_string);
    case WIFI:
      std::string wifi_menu_string = update_wifi_menu_string(current_selection);
      return std::format("{}\n{}", header_string, wifi_menu_string);
    case CLOCK:
      std::string clock_menu_string = update_clock_menu_string(current_selection);
      return std::format("{}\n{}", header_string, clock_menu_string);
    case SCHEDULE:
      std::string schedule_menu_string = update_schedule_menu_string(current_selection);
      return std::format("{}\n{}", header_string, schedule_menu_string);
    case ADD:
      std::string add_time_menu_string = update_add_time_menu_string(current_selection, add_time_hour, add_time_minute);
      return std::format("{}\n{}", header_string, add_time_menu_string);
    case DELETE:
      std::string delete_time_menu_string = update_delete_time_menu_string(current_time_selection, times, num_times);
      return std::format("{}\n{}", header_string, delete_time_menu_string);
    case VIEW:
      std::string view_times_menu_string = update_view_times_menu_string(times, num_times);
      return std::format("{}\n{}", header_string, view_times_menu_string);
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
}
} // namespace ui