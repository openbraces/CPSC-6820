#include <Arduino.h>
#include <unity.h>
#include "menu.hpp"
#include "ui.hpp"

char ui::render_string[(ui::MAX_LINE_LENGTH + 1) * ui::MAX_MENU_ENTRIES];
ui::menu::header header;

void setUp(void) { }

void tearDown(void) { }

void test_default(void) {
    String expected = "";
    String actual = String(ui::render_string);
    header.render(ui::menu::BACKWARD);
    TEST_ASSERT_EQUAL_STRING(expected.c_str(), actual.c_str());
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_default);
    UNITY_END();
}


void loop() {}
