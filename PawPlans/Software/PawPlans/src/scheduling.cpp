#include "M5DinMeter.h"

namespace scheduling {
const uint8_t MAX_SCHEDULE_ENTRIES = 16;

typedef struct entry {
  int8_t hours;
  int8_t minutes;
  float  portion;
} entry;

entry         *schedule = NULL;
uint8_t        current = 0, count = 0;
m5::rtc_time_t last_dispense;

enum status { OK,
              DUPLICATE,
              FULL };

status add_schedule_entry(int8_t hours, int8_t minutes, float portion) {
  for (uint8_t i = 0; i < count; i++) {
    if (schedule[i].hours == hours && schedule[i].minutes == minutes)
      return DUPLICATE;
  }
  if (count == 0) {
    schedule = (entry *)malloc(sizeof(entry));
  } else if (count < FULL) {
    schedule = (entry *)realloc(schedule, sizeof(entry) * ++count);
  } else {
    return FULL;
  }
  schedule[count].hours   = hours;
  schedule[count].minutes = minutes;
  schedule[count].portion = portion;

  return OK;
}

status remove_schedule_entry(uint8_t index) {
  if (count > 1) {
    memmove(&schedule[index], &schedule[index + 1], sizeof(entry) * ((count - 1) - index));
    schedule = (entry *)realloc(schedule, sizeof(entry) * --count);
  } else {
    if (schedule)
      free(schedule);
    schedule = NULL;
  }

  return OK;
}

status evaluate() {
  auto time = M5.Rtc.getTime();
  for (uint8_t i = 0; i < count; i++) {
    if (schedule[i].hours == time.hours && schedule[i].minutes == time.minutes) {
      // Dispense food.
    }
  }

  return OK;
}

status cleanup() {
  free(schedule);
}
} // namespace scheduling
