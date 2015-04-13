#pragma once
#include <pebble.h>  
  
// structure of mask for masking effects
typedef struct {
  GBitmap*  bitmap_mask; // bitmap used for mask (when masking by bitmap)
  GBitmap*  bitmap_background; // bitmap to show thru mask
  GColor    mask_color; //color of the mask
  GColor    background_color; // color of the background
  char*     text; // text used for mask (when when masking by text)
  GFont     font; // font used for text mask;
  GTextOverflowMode text_overflow; // overflow used for text mask;
  GTextAlignment  text_align; // alignment used for text masks
} EffectMask;  

typedef void effect_cb(GContext* ctx, GRect position, void* param);

// inverter effect.
// fb_a: matrix[WINDOWS_HEIGHT x WINDOWS_WIDTH] width screen bitmap data
// position: x,y,h,w of the layer
effect_cb effect_invert;

// vertical mirror effect.
// fb_a: matrix[WINDOWS_HEIGHT x WINDOWS_WIDTH] width screen bitmap data
// position: x,y,h,w of the layer
effect_cb effect_mirror_vertical;


// horizontal mirror effect.
// fb_a: matrix[WINDOWS_HEIGHT x WINDOWS_WIDTH] width screen bitmap data
// position: x,y,h,w of the layer
effect_cb effect_mirror_horizontal;

// Rotate 90 degrees
// Added by Ron64
// fb_a: matrix[WINDOWS_HEIGHT x WINDOWS_WIDTH] width screen bitmap data
// position: x,y,h,w of the layer
// right true: rotate right/clockwise false: rotate left/counter_clockwise
effect_cb effect_rotate_90_degrees;

// blur effect.
// fb_a: matrix[WINDOWS_HEIGHT x WINDOWS_WIDTH] width screen bitmap data
// position: x,y,h,w of the layer
// radius: blur radius
effect_cb effect_blur;

// mask effect.
// see struct effect_mask for parameter description
effect_cb effect_mask;
