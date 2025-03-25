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
  char   render_string[(MAX_LINE_LENGTH + 1) * MAX_MENU_ENTRIES];
  int8_t selection;
  int8_t count;

  void         render(input_event event);
  void         increment_selection();
  void         decrement_selection();
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
  void update(input_event event) override;
};

class schedule : public menu {
  void update(input_event event) override;
};

} // namespace menu
} // namespace ui

#endif // MENU_HPP