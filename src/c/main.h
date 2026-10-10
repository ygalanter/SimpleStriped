#pragma once
#include <pebble.h>

// persistent storage keys (pattern kept at 0 so settings survive upgrades)
#define PERSIST_KEY_PATTERN 0
#define PERSIST_KEY_TEXT_COLOR 1
#define PERSIST_KEY_BG_COLOR 2

// patterns: B&W are 0-9, color are 10-19 (values come from the config page)
#define PATTERN_BW_TRESHOLD 10
// no pattern: date & time drawn in solid text color over solid screen color
#define PATTERN_SOLID 20

// size of the pattern bitmaps and positions of date / time / day-of-week rows,
// relative to the top of the (centered) pattern area
#if PBL_DISPLAY_WIDTH >= 200 // emery, gabbro
  #define BACKGROUND_W 200
  #define BACKGROUND_H 220
  #define ROW_H 72
  #define FONT_SMALL RESOURCE_ID_MOLOT_64
  #define FONT_LARGE RESOURCE_ID_MOLOT_72
  #define BATTERY_BAR_H 3
  #define RING_W 5
  #ifdef PBL_ROUND
    #define ROW_DATE_Y 12
    #define ROW_TIME_Y 71
    #define ROW_DOW_Y 141
  #else
    #define ROW_DATE_Y 3
    #define ROW_TIME_Y 66
    #define ROW_DOW_Y 140
  #endif
#else // aplite, basalt, chalk, diorite, flint
  #define BACKGROUND_W 144
  #define BACKGROUND_H 160
  #define ROW_H 52
  #define FONT_SMALL RESOURCE_ID_MOLOT_46
  #define FONT_LARGE RESOURCE_ID_MOLOT_52
  #define BATTERY_BAR_H 2
  #define RING_W 4
  #ifdef PBL_ROUND
    #define ROW_DATE_Y 9
    #define ROW_TIME_Y 52
    #define ROW_DOW_Y 103
  #else
    #define ROW_DATE_Y 2
    #define ROW_TIME_Y 48
    #define ROW_DOW_Y 102
  #endif
#endif
