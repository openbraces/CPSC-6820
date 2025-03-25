#ifndef MENU_HPP
#define MENU_HPP

#include "M5DinMeter.h"

#include "ui.hpp"

namespace ui {
namespace menu {
typedef enum input_event { SELECT, FORWARD, BACKWARD } input_event;

class menu {
public:
  static const char SELECTION_PREFIX = '>';

  char   title[MAX_LINE_LENGTH];
  char   entries[MAX_LINE_LENGTH][MAX_MENU_ENTRIES];
  int8_t selection;
  int8_t count;

  // Render will be called each time during the event loop
  void render(input_event event);
  // Move the selection marker on the entries forward reseting the selection to
  // 0 when past count - 1
  void increment_selection();
  // Move the selection marker backward, similar logic as increment_selection()
  void decrement_selection();

protected:
  // Update function to adjust the entries of the list, needs to be defined by
  // the specific menus
  virtual void update(input_event event) = 0;
};

class header : public menu {
  void update(input_event event) override;
};

class root : public menu {
  void update(input_event event) override;
};

class wifi : public menu {
  void update(input_event event) override;
};

class clock : public menu {
private:
  bool             time_lock;
  scheduling::time time;

public:
  static const char sub_selection_prefix[], sub_selection_suffix[];

  void update(input_event event) override;
};

class schedule : public menu {
  void update(input_event event) override;
};

} // namespace menu
} // namespace ui

#endif // MENU_HPP