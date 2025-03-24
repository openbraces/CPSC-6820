#include "M5DinMeter.h"

#include "scheduling.hpp"

namespace ui {
inline const auto UI_SELECTION_PREFIX = '<';

bool wifi_enabled   = false;
bool overall_status = true;

void init() {
  DinMeter.Display.setRotation(1);
  DinMeter.Display.setTextColor(GREEN);
  DinMeter.Display.setTextDatum(middle_center);
  DinMeter.Display.setTextFont(&fonts::Roboto_Thin_24);
  DinMeter.Display.setTextSize(2);

  auto encpos = DinMeter.Encoder.read();
}

char *format_time(scheduling::time time) {
  char time_string[5];
  sprintf(time_string, "%02d:%02d", time.hours, time.minutes);

  return time_string;
}

inline const auto max = [](int8_t x, int8_t y) { return x > y ? x : y; };
inline const auto min = [](int8_t x, int8_t y) { return x < y ? y : x; };
namespace menus {
class menu {
public:
  static const inline int8_t MAX_MENU_ENTRIES = 8, MAX_LINE_LENGTH = 64;
  static const inline char   selection_prefix = '>';

  typedef enum event { SELECT, FORWARD, BACKWARD } event;

  char   title[MAX_LINE_LENGTH];
  char   entries[MAX_LINE_LENGTH][MAX_MENU_ENTRIES];
  char   render_string[(MAX_LINE_LENGTH + 1) * MAX_MENU_ENTRIES];
  int8_t selection;
  int8_t count;

  // Render will be called each time during the event loop
  void render(event event) {
    // Internally, we call the update function that is defined by each menu
    update(event);
    // Writing the title first
    sprintf(render_string, "%s\n", title);
    for (int8_t i = 0; i < count; i++) {
      // Beginning here, we append to the render_string, each line having one
      // entry
      if (i == selection)
        sprintf(render_string + strlen(render_string), "%c %.*s\n",
                selection_prefix, MAX_LINE_LENGTH - 2, entries[i]);
      else
        sprintf(render_string + strlen(render_string), "%.*s\n",
                MAX_LINE_LENGTH, entries[i]);
    }
  }

  // Move the selection marker on the entries forward reseting the selection to 0 when past count - 1
  void increment_selection() { selection = (selection + 1) % count; }

  // Move the selection marker backward, similar logic as increment_selection()
  void decrement_selection() { selection = (selection - 1) % count; }

  // Update function to adjust the entries of the list, needs to be defined by
  // the specific menus
  virtual void update(event event) = 0;
};

class header : menu {
  void update(event event) {
    sprintf(entries[0], "WiFi %s T %02d:%02d PRE %02d:%02d NXT %02d:%02d %s",
            wifi_enabled ? "ON " : "OFF",
            format_time(scheduling::get_current_time()),
            format_time(scheduling::last_dispense),
            format_time(scheduling::next_dispense),
            overall_status ? "OK" : "ERR");
    count = 1;
  }
};

class root : menu {
  void update(event event) {
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
    sprintf(title, "PawPlans");
    sprintf(entries[0], "WiFi");
    sprintf(entries[1], "Clock");
    sprintf(entries[2], "Schedule");
    count = 3;
  }
};

class wifi : menu {
  void update(event event) {
    sprintf(title, "WiFi");
    sprintf(entries[0], "TBD");
    count = 1;
  }
};

class clock : menu {
  static inline const char sub_selection_prefix[] = "[",
                           sub_selection_suffix[] = "]";

  scheduling::time time = scheduling::get_current_time();

  void update(event event) {
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
        scheduling::set_system_time(time.hours, time.minutes);
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

class schedule : menu {
  void update(event event) {
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
} // namespace menus

void update() {
  DinMeter.Speaker.tone(8000, 20);
  DinMeter.Display.clear();
  Serial.println(); // For debugging
  DinMeter.Display.drawString("", 0,
                              0); // x = 0, y = 0 should be top left of screen

  if (DinMeter.BtnA.wasPressed()) {
    DinMeter.Encoder.readAndReset();
  }
  if (DinMeter.BtnA.pressedFor(5000)) {
    DinMeter.Encoder.write(100);
  }
}
} // namespace ui