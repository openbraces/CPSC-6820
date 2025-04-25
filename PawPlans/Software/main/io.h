#pragma once

extern float curr_servo_angle;

void io_init(void);

// Ultrasonic sensor distance in cm
float get_distance(void);

// Load cell's reading in grams
float get_weight(void);

// Sets servo angle in degrees (0–180)
void set_servo_angle(float angle);

void servo_dispense(int portion);

void buzzer_beep(void);
