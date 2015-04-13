#include <pebble.h>
#include "effect_layer.h"

Window *my_window;

TextLayer *text_layer;
EffectLayer* effect_layer;
static EffectMask mask;

char time_date[] = "APR12 16:03 SUN"; //test

void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  
   if (!clock_is_24h_style()) {
    
        if( tick_time->tm_hour > 11 ) {   // YG Jun-25-2014: 0..11 - am 12..23 - pm
            if( tick_time->tm_hour > 12 ) tick_time->tm_hour -= 12;
        } else {
            if( tick_time->tm_hour == 0 ) tick_time->tm_hour = 12;
        }        
    }
 
    //time & date
    strftime(time_date, sizeof(time_date), "%b%d %H:%M %a", tick_time);
    text_layer_set_text(text_layer, time_date);
  
    
  
}

void handle_init(void) {
  my_window = window_create();
  
  #ifdef PBL_COLOR
    window_set_background_color(my_window, GColorOxfordBlue);
  #else 
    window_set_background_color(my_window, GColorBlack);
  #endif  
  window_stack_push(my_window, true);
  
  text_layer = text_layer_create(GRect(0,0,144,168));
  text_layer_set_font(text_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_MOLOT_50)));
  text_layer_set_text_color(text_layer, GColorWhite);  
  text_layer_set_background_color(text_layer, GColorClear);  
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  
  layer_add_child(window_get_root_layer(my_window), text_layer_get_layer(text_layer));
    
  //creating effect layer
  effect_layer = effect_layer_create(GRect(0,0,144,168));
  
  
  // ** { begin setup mask for MASK effect
  mask.text = NULL;
  mask.bitmap_mask = NULL;
  mask.mask_color = GColorWhite;
  mask.background_color = GColorClear;
  mask.bitmap_background = gbitmap_create_with_resource(RESOURCE_ID_MASC_BG);
  // ** end setup mask }

  effect_layer_add_effect(effect_layer, effect_mask, &mask);
  
  layer_add_child(window_get_root_layer(my_window), effect_layer_get_layer(effect_layer));
  
  tick_timer_service_subscribe(MINUTE_UNIT, (TickHandler) tick_handler);
    
}

void handle_deinit(void) {
  
  //clearning MASK
  gbitmap_destroy(mask.bitmap_background);
  effect_layer_destroy(effect_layer);
  text_layer_destroy(text_layer);
  
  window_destroy(my_window);
  tick_timer_service_unsubscribe();
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
