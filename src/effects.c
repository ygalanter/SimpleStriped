#include <pebble.h>
#include "effects.h"
  
// set pixel color at given coordinates 
void set_pixel(uint8_t *bitmap_data, int bytes_per_row, int y, int x, uint8_t color) {
  
  uint8_t (*fb_a)[bytes_per_row] = (uint8_t (*)[bytes_per_row])bitmap_data;
  
  #ifdef PBL_COLOR 
    fb_a[y][x] = color; // in Basalt - simple set entire byte
  #else
    fb_a[y][x / 8] ^= (-color ^ fb_a[y][x / 8]) & (1 << (x % 8)); // in Applite - set the bit
  #endif
}




// get pixel color at given coordinates 
uint8_t get_pixel(uint8_t *bitmap_data, int bytes_per_row, int y, int x) {
  
  uint8_t (*fb_a)[bytes_per_row] = (uint8_t (*)[bytes_per_row])bitmap_data;
  
  #ifdef PBL_COLOR
    return fb_a[y][x]; // in Basalt - simple get entire byte
  #else
    return (fb_a[y][x / 8] >> (x % 8)) & 1; // in Applite - get the bit
  #endif
}
  
  

// inverter effect.
// fb_a: matrix[WINDOWS_HEIGHT x WINDOWS_WIDTH] width screen bitmap data
// position: x,y,h,w of the layer  
void effect_invert(GContext* ctx,  GRect position, void* param) {
  //capturing framebuffer bitmap
  GBitmap *fb = graphics_capture_frame_buffer(ctx);
  uint8_t *bitmap_data =  gbitmap_get_data(fb);
  int bytes_per_row = gbitmap_get_bytes_per_row(fb);

  
  for (int y = 0; y < position.size.h; y++)
     for (int x = 0; x < position.size.w; x++)
        #ifdef PBL_COLOR // on Basalt simple doing NOT on entire returned byte/pixel
          set_pixel(bitmap_data, bytes_per_row, y + position.origin.y, x + position.origin.x, ~get_pixel(bitmap_data, bytes_per_row, y + position.origin.y, x + position.origin.x));
        #else // on applite since only 1 and 0 is returning, doing "not" by 1 - pixel
          set_pixel(bitmap_data, bytes_per_row, y + position.origin.y, x + position.origin.x, 1 - get_pixel(bitmap_data, bytes_per_row, y + position.origin.y, x + position.origin.x));
        #endif
 
  graphics_release_frame_buffer(ctx, fb);          
          
}

// vertical mirror effect.
// fb_a: matrix[WINDOWS_HEIGHT x WINDOWS_WIDTH] width screen bitmap data
// position: x,y,h,w of the layer
void effect_mirror_vertical(GContext* ctx, GRect position, void* param) {
  uint8_t temp_pixel;  
  
  //capturing framebuffer bitmap
  GBitmap *fb = graphics_capture_frame_buffer(ctx);
  uint8_t *bitmap_data =  gbitmap_get_data(fb);
  int bytes_per_row = gbitmap_get_bytes_per_row(fb);

  for (int y = 0; y < position.size.h / 2 ; y++)
     for (int x = 0; x < position.size.w; x++){
        temp_pixel = get_pixel(bitmap_data, bytes_per_row, y + position.origin.y, x + position.origin.x);
        set_pixel(bitmap_data, bytes_per_row, y + position.origin.y, x + position.origin.x, get_pixel(bitmap_data, bytes_per_row, position.origin.y + position.size.h - y - 2, x + position.origin.x));
        set_pixel(bitmap_data, bytes_per_row, position.origin.y + position.size.h - y - 2, x + position.origin.x, temp_pixel);
     }
  
  graphics_release_frame_buffer(ctx, fb);
}


// horizontal mirror effect.
// fb_a: matrix[WINDOWS_HEIGHT x WINDOWS_WIDTH] width screen bitmap data
// position: x,y,h,w of the layer
void effect_mirror_horizontal(GContext* ctx, GRect position, void* param) {
  uint8_t temp_pixel;  
  
  //capturing framebuffer bitmap
  GBitmap *fb = graphics_capture_frame_buffer(ctx);
  uint8_t *bitmap_data =  gbitmap_get_data(fb);
  int bytes_per_row = gbitmap_get_bytes_per_row(fb);


  for (int y = 0; y < position.size.h; y++)
     for (int x = 0; x < position.size.w / 2; x++){
        temp_pixel = get_pixel(bitmap_data, bytes_per_row, y + position.origin.y, x + position.origin.x);
        set_pixel(bitmap_data, bytes_per_row, y + position.origin.y, x + position.origin.x, get_pixel(bitmap_data, bytes_per_row, y + position.origin.y, position.origin.x + position.size.w - x - 2));
        set_pixel(bitmap_data, bytes_per_row, y + position.origin.y, position.origin.x + position.size.w - x - 2, temp_pixel);
     }
  
  graphics_release_frame_buffer(ctx, fb);
}

// Rotate 90 degrees
// Added by Ron64
// fb_a: matrix[WINDOWS_HEIGHT x WINDOWS_WIDTH] width screen bitmap data
// position: x,y,h,w of the layer
// right true: rotate right/clockwise false: rotate left/counter_clockwise
void effect_rotate_90_degrees(GContext* ctx,  GRect position, void* param){

  //capturing framebuffer bitmap
  GBitmap *fb = graphics_capture_frame_buffer(ctx);
  uint8_t *bitmap_data =  gbitmap_get_data(fb);
  int bytes_per_row = gbitmap_get_bytes_per_row(fb);

  bool right = (bool)param;
  uint8_t qtr, xCn, yCn, temp_pixel;
  xCn= position.origin.x + position.size.w /2;
  yCn= position.origin.y + position.size.h /2;
  qtr=position.size.w;
  if (position.size.h < qtr)
    qtr= position.size.h;
  qtr= qtr/2;

  for (int c1 = 0; c1 < qtr; c1++)
    for (int c2 = 1; c2 < qtr; c2++){
      temp_pixel = get_pixel(bitmap_data, bytes_per_row, yCn +c1, xCn +c2);
      if (right){
        set_pixel(bitmap_data, bytes_per_row, yCn +c1, xCn +c2, get_pixel(bitmap_data, bytes_per_row, yCn -c2, xCn +c1));
        set_pixel(bitmap_data, bytes_per_row, yCn -c2, xCn +c1, get_pixel(bitmap_data, bytes_per_row, yCn -c1, xCn -c2));
        set_pixel(bitmap_data, bytes_per_row, yCn -c1, xCn -c2, get_pixel(bitmap_data, bytes_per_row, yCn +c2, xCn -c1));
        set_pixel(bitmap_data, bytes_per_row, yCn +c2, xCn -c1, temp_pixel);
      }
      else{
        set_pixel(bitmap_data, bytes_per_row, yCn +c1, xCn +c2, get_pixel(bitmap_data, bytes_per_row, yCn +c2, xCn -c1));
        set_pixel(bitmap_data, bytes_per_row, yCn +c2, xCn -c1, get_pixel(bitmap_data, bytes_per_row, yCn -c1, xCn -c2));
        set_pixel(bitmap_data, bytes_per_row, yCn -c1, xCn -c2, get_pixel(bitmap_data, bytes_per_row, yCn -c2, xCn +c1));
        set_pixel(bitmap_data, bytes_per_row, yCn -c2, xCn +c1, temp_pixel);
      }
     }
  
  graphics_release_frame_buffer(ctx, fb);
}
  
// mask effect.
// see struct effect_mask for parameter description  
void effect_mask(GContext* ctx, GRect position, void* param) {
  GColor temp_pixel;  
  EffectMask *mask = (EffectMask *)param;

  //drawing background
  if (!GColorEq(mask->background_color, GColorClear)) {
    graphics_context_set_fill_color(ctx, mask->background_color);
    graphics_context_set_stroke_color(ctx, mask->mask_color);
    graphics_fill_rect(ctx, GRect(0, 0, position.size.w, position.size.h), 0, GCornerNone); 
  }
  
  if (mask->text) {//if text mask is used - drawing text
     graphics_draw_text(ctx, mask->text, mask->font, GRect(0, 0, position.size.w, position.size.h), mask->text_overflow, mask->text_align, NULL);
  } else if (mask->bitmap_mask) { // othersise - bitmap mask is used
     graphics_draw_bitmap_in_rect(ctx, mask->bitmap_mask, GRect(0, 0, position.size.w, position.size.h));
  }
    
  //capturing framebuffer bitmap
  GBitmap *fb = graphics_capture_frame_buffer(ctx);
  uint8_t *bitmap_data =  gbitmap_get_data(fb);
  int bytes_per_row = gbitmap_get_bytes_per_row(fb);
  
  //capturing background bitmap
  uint8_t *bg_bitmap_data =  gbitmap_get_data(mask->bitmap_background);
  int bg_bytes_per_row = gbitmap_get_bytes_per_row(mask->bitmap_background);
    
  //looping thru layer replacing mask with bg bitmap
  for (int y = 0; y < position.size.h; y++)
     for (int x = 0; x < position.size.w; x++) {
       temp_pixel = (GColor)get_pixel(bitmap_data, bytes_per_row, y + position.origin.y, x + position.origin.x);
       if (GColorEq(temp_pixel, mask->mask_color))
         set_pixel(bitmap_data, bytes_per_row, y + position.origin.y, x + position.origin.x, get_pixel(bg_bitmap_data, bg_bytes_per_row, y + position.origin.y, x + position.origin.x));
  }
  
  graphics_release_frame_buffer(ctx, fb);
  
}