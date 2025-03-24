#include "M5DinMeter.h"

#include "scheduling.hpp"

namespace scheduling {
entry   schedule[MAX_SCHEDULE_ENTRIES];
uint8_t count = 0;
time    last_dispense, next_dispense;

int compare_schedule_entries(const void *x, const void *y) {
  const auto xtime = ((const entry *)x)->time, ytime = ((const entry *)y)->time;
  return (xtime.hours * 60 + xtime.minutes > ytime.hours * 60 + ytime.minutes) -
         (xtime.hours * 60 + xtime.minutes < ytime.hours * 60 + ytime.minutes);
}

time get_current_time() {
  const auto rtctime      = M5.Rtc.getTime();
  const time current_time = {rtctime.hours, rtctime.minutes};

  return current_time;
}

void set_system_time(int8_t hours, int8_t minutes) {
  const m5::rtc_time_t time{hours, minutes};
  M5.Rtc.setTime(time);
}

status add_schedule_entry(int8_t hours, int8_t minutes, float portion) {
  if (count == MAX_SCHEDULE_ENTRIES) {
    return FULL;
  }
  // Check if an entry already exists for this time
  for (uint8_t i = 0; i < count; i++) {
    if (schedule[i].time.hours == hours && schedule[i].time.minutes == minutes)
      return DUPLICATE;
  }

  schedule[count].time.hours   = hours;
  schedule[count].time.minutes = minutes;
  schedule[count].portion      = portion;

  count++;

  qsort(schedule, count, sizeof(entry), compare_schedule_entries);

  return OK;
}

status remove_schedule_entry(uint8_t index) {
  schedule[index].time.hours   = INT8_MAX;
  schedule[index].time.minutes = INT8_MAX;
  schedule[index].portion      = 0;

  count--;

  qsort(schedule, count, sizeof(entry), compare_schedule_entries);

  return OK;
}

status evaluate() {
  const auto time = M5.Rtc.getTime(); // Get the current time

  // Loop through the schedule entries to check if any entry matches the current
  // time
  for (uint8_t i = 0; i < count; i++) {
    if (schedule[i].time.hours == time.hours &&
        schedule[i].time.minutes == time.minutes) {
      // TODO: Dispense food
      last_dispense = get_current_time();
      next_dispense = schedule[i].time;
    }
  }

  return OK;
}

status cleanup() {
  free(schedule);
  return OK;
}

} // namespace scheduling
