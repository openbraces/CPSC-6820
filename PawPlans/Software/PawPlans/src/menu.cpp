#include "M5DinMeter.h"

#include "menu.hpp"
#include "ui.hpp"

namespace ui {
namespace menu {
void menu::render(input_event event) {
  // Internally, we call the update function that is defined by each menu
  update(event);
  // Writing the title first
  sprintf(render_string + strlen(render_string), "%s\n", title);
  for (int8_t i = 0; i < count; i++) {
    // Beginning here, we append to the render_string, each line having one
    // entry
    if (i == selection)
      sprintf(render_string + strlen(render_string), "%c %.*s\n",
              SELECTION_PREFIX, MAX_LINE_LENGTH - 2, entries[i]);
    else
      sprintf(render_string + strlen(render_string), "%.*s\n", MAX_LINE_LENGTH,
              entries[i]);
  }
}

void menu::increment_selection() {
  selection = (selection + 1) < count ? selection + 1 : 0;
}

void menu::decrement_selection() {
  selection = (selection - 1) > 0 ? selection - 1 : count - 1;
}

void header::update(input_event event) {
  char current_time[5], last_dispense_time[5], next_dispense_time[5];
  format_time(scheduling::get_current_time(), current_time);
  format_time(scheduling::last_dispense, last_dispense_time);
  format_time(scheduling::next_dispense, next_dispense_time);
  sprintf(entries[0], "WiFi %s T %02d:%02d PRE %02d:%02d NXT %02d:%02d %s",
          ui::wifi_status ? "ON " : "OFF", current_time, last_dispense_time,
          next_dispense_time, system_status ? "OK" : "ERR");
  count = 1;
}

void root::update(input_event event) {
  switch (event) {
  case SELECT:
    switch (selection) {
    // WiFi case
    case 0:
      current_menu = WIFI;
    // Clock case
    case 1:
      current_menu = CLOCK;
    // Schedule case
    case 2:
      current_menu = SCHEDULE;
    }
    break;

  case FORWARD:
    increment_selection();
    break;
  case BACKWARD:
    increment_selection();
    break;
  }
  sprintf(title, "PawPlans");
  sprintf(entries[0], "WiFi");
  sprintf(entries[1], "Clock");
  sprintf(entries[2], "Schedule");
  count = 3;
}

void wifi::update(input_event event) {
  sprintf(title, "WiFi");
  sprintf(entries[0], "TBD");
  count = 1;
}

const char clock::sub_selection_prefix[] = "[",
           clock::sub_selection_suffix[] = "]";

void clock::update(input_event event) {
  static char hours[4], minutes[4];
  enum { HOURS, MINUTES, DESELECT } time_selection = DESELECT;

  if (!time_lock) {
    time = scheduling::get_current_time();
  }

  // Menu entry switch
  switch (selection) {
  // Time setting
  case 0:
    // Event switch
    switch (event) {
    case SELECT:
      switch (time_selection) {
      case DESELECT:
        time_selection = HOURS;
        time_lock      = true;
        break;
      case HOURS:
        time_selection = MINUTES;
        break;
      case MINUTES:
        time_selection = DESELECT;
        break;
      }
      break;
    case FORWARD:
      switch (time_selection) {
      case DESELECT:
        increment_selection();
        break;
      case HOURS:
        time.hours = min(time.hours + 1, 0);
        break;
      case MINUTES:
        time.minutes = min(time.minutes - 1, 0);
        break;
      }
      break;
    case BACKWARD:
      switch (time_selection) {
      case DESELECT:
        decrement_selection();
        break;
      case HOURS:
        time.hours = max(time.hours - 1, 0);
        break;
      case MINUTES:
        time.minutes = max(time.minutes - 1, 0);
        break;
      }
      break;
    }
    break;
  // Set
  case 1:
    switch (event) {
    case SELECT:
      scheduling::set_system_time(time.hours, time.minutes);
      time_lock = false;
      break;
    case FORWARD:
      increment_selection();
      break;
    case BACKWARD:
      decrement_selection();
      break;
    }
  // Cancel
  case 2:
    switch (event) {
    case SELECT:
      current_menu = ROOT;
      break;
    case FORWARD:
      increment_selection();
      break;
    case BACKWARD:
      decrement_selection();
      break;
    }
  }

  sprintf(hours, "%s%02d%s",
          time_selection == HOURS ? sub_selection_prefix : "", time.hours,
          time_selection == HOURS ? sub_selection_suffix : "");
  sprintf(minutes, "%s%02d%s",
          time_selection == MINUTES ? sub_selection_prefix : "", time.minutes,
          time_selection == MINUTES ? sub_selection_suffix : "");

  sprintf(title, "Clock");
  sprintf(entries[0], "%s:%s", hours, minutes);
  sprintf(entries[1], "Set");
  sprintf(entries[2], "Cancel");
  count = 3;
}

void schedule::update(input_event event) {
  switch (event) {
  case SELECT:
    switch (selection) {
      // Write selection cases
    }
    break;

  case FORWARD:
    increment_selection();
    break;
  case BACKWARD:
    increment_selection();
    break;
  }
  sprintf(title, "Schedule");
  sprintf(entries[0], "Add");
  sprintf(entries[1], "View");
  sprintf(entries[2], "Delete");
  sprintf(entries[3], "Cancel");
  count = 4;
}
} // namespace menu
} // namespace ui