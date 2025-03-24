#ifndef SCHEDULING_H
#define SCHEDULING_H

#include <M5DinMeter.h>
#include <stdint.h>

namespace scheduling {
// Maximum number of schedule entries
inline const uint8_t MAX_SCHEDULE_ENTRIES = 16;

// Schedule time
typedef struct time {
  int8_t hours;   // Hour of the day
  int8_t minutes; // Minute of the day
} time;

// Schedule entry
typedef struct entry {
  time  time;    // Time of the day
  float portion; // Portion size for the schedule
} entry;

extern entry  *schedule;
extern uint8_t count;
extern time    last_dispense, next_dispense;

// Status codes for schedule manipulation operations
enum status {
  OK,        // OK
  DUPLICATE, // Duplicate schedule entry
  FULL       // Maximum entries reached
};

// Get current system time, in hours and minutes
time get_current_time();

// Set current system time, in hours and minutes
void set_system_time(int8_t hours, int8_t minutes);

// Add a schedule entry
status add_schedule_entry(int8_t hours, int8_t minutes, float portion);

// Remove a schedule entry by index
status remove_schedule_entry(uint8_t index);

// Evaluate the entries in the schedule and dispense food
status evaluate();

// Memory cleanup
status cleanup();
} // namespace scheduling

#endif