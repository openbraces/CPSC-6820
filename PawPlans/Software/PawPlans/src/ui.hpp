#ifndef UI_HPP
#define UI_HPP

#include "M5DinMeter.h"

#include "scheduling.hpp"

namespace ui {
static const int8_t MAX_MENU_ENTRIES = 8, MAX_LINE_LENGTH = 64;

enum menutype { ROOT, WIFI, CLOCK, SCHEDULE };

extern bool          wifi_status;
extern bool          system_status;
extern enum menutype current_menu;

void format_time(scheduling::time time, char time_string[5]);
} // namespace ui

#endif // UI_HPP
