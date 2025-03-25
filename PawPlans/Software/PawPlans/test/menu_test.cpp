#include <Arduino.h>
#include <unity.h>
#include "menu.hpp"
#include "ui.hpp"

char ui::render_string[(ui::MAX_LINE_LENGTH + 1) * ui::MAX_MENU_ENTRIES];
ui::menu::header header;
ui::menu::root root;
ui::menu::clock clock;

void setUp(void) {
    header = ui::menu::header{};
    root = ui::menu::root{};
    clock = ui::menu::clock{};
}

void tearDown(void) { }

void test_header(void) {
    String expected = "WiFi OFF T 00:00 PRE 00:00 NXT 00:00 OK\nPawPlans\nWiFi\nClock\nSchedule";
    header.render(ui::menu::BACKWARD);
    String actual = String(ui::render_string);
    TEST_ASSERT_EQUAL_STRING(expected.c_str(), actual.c_str());
}

void test_select_wifi(void) {
    String expected = "WiFi OFF T 00:00 PRE 00:00 NXT 00:00 OK\nWiFi\nTBD";
    root.render(ui::menu::SELECT);
    String actual = String(ui::render_string);
    TEST_ASSERT_EQUAL_STRING(expected.c_str(), actual.c_str());
}

void test_select_clock(void) {
    String expected = "WiFi OFF T 00:00 PRE 00:00 NXT 00:00 OK\nClock\n[00]:00\nSet\nCancel";
    root.render(ui::menu::FORWARD);
    root.render(ui::menu::SELECT);
    String actual = String(ui::render_string);
    TEST_ASSERT_EQUAL_STRING(expected.c_str(), actual.c_str());
}

void test_select_schedule(void) {
    String expected = "WiFi OFF T 00:00 PRE 00:00 NXT 00:00 OK\nSchedule\nAdd\nView\nDelete\nCancel";
    root.render(ui::menu::FORWARD);
    root.render(ui::menu::FORWARD);
    root.render(ui::menu::SELECT);
    String actual = String(ui::render_string);
    TEST_ASSERT_EQUAL_STRING(expected.c_str(), actual.c_str());
}

void test_clock_select_minutes(void) {
    String expected = "WiFi OFF T 00:00 PRE 00:00 NXT 00:00 OK\nClock\n00:[00]\nSet\nCancel";
    root.render(ui::menu::FORWARD);
    String actual = String(ui::render_string);
    TEST_ASSERT_EQUAL_STRING(expected.c_str(), actual.c_str());
}

void test_clock_increment_hour(void) {
    String expected = "WiFi OFF T 00:00 PRE 00:00 NXT 00:00 OK\nClock\n[01]:00\nSet\nCancel";
    root.render(ui::menu::SELECT);
    root.render(ui::menu::FORWARD);
    String actual = String(ui::render_string);
    TEST_ASSERT_EQUAL_STRING(expected.c_str(), actual.c_str());
}

void test_clock_decrement_hour(void) {
    String expected = "WiFi OFF T 00:00 PRE 00:00 NXT 00:00 OK\nClock\n[23]:00\nSet\nCancel";
    root.render(ui::menu::SELECT);
    root.render(ui::menu::BACKWARD);
    String actual = String(ui::render_string);
    TEST_ASSERT_EQUAL_STRING(expected.c_str(), actual.c_str());
}

void test_clock_increment_minutes(void) {
    String expected = "WiFi OFF T 00:00 PRE 00:00 NXT 00:00 OK\nClock\n00:[01]\nSet\nCancel";
    root.render(ui::menu::FORWARD);
    root.render(ui::menu::SELECT);
    root.render(ui::menu::FORWARD);
    String actual = String(ui::render_string);
    TEST_ASSERT_EQUAL_STRING(expected.c_str(), actual.c_str());
}

void test_clock_decrement_minutes(void) {
    String expected = "WiFi OFF T 00:00 PRE 00:00 NXT 00:00 OK\nClock\n00:[59]\nSet\nCancel";
    root.render(ui::menu::FORWARD);
    root.render(ui::menu::SELECT);
    root.render(ui::menu::BACKWARD);
    String actual = String(ui::render_string);
    TEST_ASSERT_EQUAL_STRING(expected.c_str(), actual.c_str());
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_header);
    RUN_TEST(test_select_wifi);
    RUN_TEST(test_select_clock);
    RUN_TEST(test_select_schedule);
    RUN_TEST(test_clock_select_minutes);
    RUN_TEST(test_clock_increment_hour);
    RUN_TEST(test_clock_decrement_hour);
    RUN_TEST(test_clock_increment_minutes);
    RUN_TEST(test_clock_decrement_minutes);
    UNITY_END();
}

void loop() {}
