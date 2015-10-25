#include <pebble.h>

// flags
#define KEY_PATTERN 0  
#define PATTERN_BW_TRESHOLD 10 // where b&w images stop and color start  
  
//array to hold patterns: B&W (0-9), Color (10-19); for now only 4 are used
int pattern_array[20] = 
  { RESOURCE_ID_HORIZONTAL_BW,
    RESOURCE_ID_VERTICAL_BW,
    RESOURCE_ID_TV_NOISE_BW,
    RESOURCE_ID_GRID_BW, 
    0, 0, 0, 0, 0, 0, 
    #ifdef PBL_COLOR
      RESOURCE_ID_HORIZONTAL_COLOR,
      RESOURCE_ID_VERTICAL_COLOR,
      RESOURCE_ID_TV_NOISE_COLOR,
      RESOURCE_ID_SUNSET_COLOR, 
      0, 0, 0, 0, 0, 0
    #endif    
  };

#define BACKGROUND_W 130
#define BACKGROUND_H 160