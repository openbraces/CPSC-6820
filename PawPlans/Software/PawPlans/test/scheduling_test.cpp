#include "scheduling.hpp"
#include <Arduino.h>
#include <unity.h>

void setUp(void) {}

void tearDown(void) {}

void test_add_schedule_entry(void) {
  int8_t expected_hour    = 0;
  int8_t expected_minute  = 0;
  int8_t expected_portion = 1.0;

  scheduling::add_schedule_entry(0, 0, 1.0);

  int8_t actual_hour    = scheduling::schedule[0].time.hours;
  int8_t actual_minute  = scheduling::schedule[0].time.minutes;
  float  actual_portion = scheduling::schedule[0].portion;

  TEST_ASSERT_EQUAL_INT8(expected_hour, actual_hour);
  TEST_ASSERT_EQUAL_INT8(expected_minute, actual_minute);
  TEST_ASSERT_EQUAL_FLOAT(expected_portion, actual_portion);
}

void test_remove_schedule_entry(void) {
  int8_t expected_hour    = 0;
  int8_t expected_minute  = 0;
  int8_t expected_portion = 1.0;

  // Add the entries
  scheduling::add_schedule_entry(12, 12, 2.0);
  scheduling::add_schedule_entry(0, 0, 1.0);

  // Remove the first index (should replace first entry with second)
  scheduling::remove_schedule_entry(0);

  int8_t actual_hour    = scheduling::schedule[0].time.hours;
  int8_t actual_minute  = scheduling::schedule[0].time.minutes;
  float  actual_portion = scheduling::schedule[0].portion;

  TEST_ASSERT_EQUAL_INT8(expected_hour, actual_hour);
  TEST_ASSERT_EQUAL_INT8(expected_minute, actual_minute);
  TEST_ASSERT_EQUAL_FLOAT(expected_portion, actual_portion);
}

void setup() {
  UNITY_BEGIN();
  RUN_TEST(test_add_schedule_entry);
  RUN_TEST(test_remove_schedule_entry);
  UNITY_END();
}

void loop() {}
