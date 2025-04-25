#pragma once

#include <sys/time.h>
#include <time.h>

#include "esp_lvgl_port.h"

#define CLOCK_HOUR_OPTIONS                                                     \
  "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18" \
  "\n19\n20\n21\n22\n23"
#define CLOCK_MINUTE_OPTIONS                                                   \
  "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18" \
  "\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31\n32\n33\n34\n35\n36\n" \
  "37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n51\n52\n53\n54\n55" \
  "\n56\n57\n58\n59"
#define CLOCK_SECOND_OPTIONS                                                   \
  "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18" \
  "\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31\n32\n33\n34\n35\n36\n" \
  "37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n51\n52\n53\n54\n55" \
  "\n56\n57\n58\n59"

void clock_create(lv_obj_t *parent);
