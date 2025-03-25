#include "M5DinMeter.h"

#include "menu.hpp"
#include "ui.hpp"

namespace ui {
namespace menu {
class menu {
public:
  static const char SELECTION_PREFIX = '>';

  char   title[MAX_LINE_LENGTH];
  char   entries[MAX_LINE_LENGTH][MAX_MENU_ENTRIES];
  char   render_string[(MAX_LINE_LENGTH + 1) * MAX_MENU_ENTRIES];
  int8_t selection;
  int8_t count;

  // Render will be called each time during the event loop
  void render(input_event event) {
    // Internally, we call the update function that is defined by each menu
    update(event);
    // Writing the title first
    sprintf(render_string, "%s\n", title);
    for (int8_t i = 0; i < count; i++) {
      // Beginning here, we append to the render_string, each line having one
      // entry
      if (i == selection)
        sprintf(render_string + strlen(render_string), "%c %.*s\n",
                SELECTION_PREFIX, MAX_LINE_LENGTH - 2, entries[i]);
      else
        sprintf(render_string + strlen(render_string), "%.*s\n",
                MAX_LINE_LENGTH, entries[i]);
    }
  }

  // Move the selection marker on the entries forward reseting the selection to
  // 0 when past count - 1
  void increment_selection() {
    selection = (selection + 1) < count ? selection + 1 : 0;
  }

  // Move the selection marker backward, similar logic as increment_selection()
  void decrement_selection() {
    selection = (selection - 1) > 0 ? selection - 1 : count - 1;
  }

  // Update function to adjust the entries of the list, needs to be defined by
  // the specific menus
  virtual void update(input_event event) = 0;
};

class header : public menu {
  void update(input_event event) override {
    char current_time[5], last_dispense_time[5], next_dispense_time[5];
    format_time(scheduling::get_current_time(), current_time);
    format_time(scheduling::last_dispense, last_dispense_time);
    format_time(scheduling::next_dispense, next_dispense_time);
    sprintf(entries[0], "WiFi %s T %02d:%02d PRE %02d:%02d NXT %02d:%02d %s",
            ui::wifi_status ? "ON " : "OFF", current_time, last_dispense_time,
            next_dispense_time, system_status ? "OK" : "ERR");
    count = 1;
  }
};

class root : public menu {
  void update(input_event event) override {
    switch (event) {
    case SELECT:
      switch (selection) {
      // Wifi case
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
};

class wifi : public menu {
  void update(input_event event) override {
    sprintf(title, "WiFi");
    sprintf(entries[0], "TBD");
    count = 1;
  }
};

class clock : public menu {
  static constexpr char sub_selection_prefix[] = "[",
                        sub_selection_suffix[] = "]";

  scheduling::time time = scheduling::get_current_time();

  void update(input_event event) override {
    static char hours[4], minutes[4];
    enum { HOURS, MINUTES, DESELECT } time_selection = DESELECT;

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
};

class schedule : public menu {
  void update(input_event event) override {
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
};
} // namespace menu
} // namespace ui