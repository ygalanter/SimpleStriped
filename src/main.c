#include <pebble.h>
#include "main.h"  
#include "effect_layer.h"

Window *my_window;

TextLayer *text_time, *text_date, *text_dow;
BitmapLayer *battery_layer;
EffectLayer* effect_layer;
static EffectMask mask;

char s_date[] = "HELLO"; //test
char s_time[] = "HOWRE"; //test
char s_dow[] = "YOU"; //test

int current_pattern; // currently loaded pattern

// changes pattern of background
static void change_pattern(int pattern) {
  
  // destroying existing pattern
  if (mask.bitmap_background != NULL) {
    gbitmap_destroy(mask.bitmap_background);
  }
  
  // and creating new one
  mask.bitmap_background = gbitmap_create_with_resource(pattern_array[pattern]);
  
  //for b&w patterns do a b&w background
  #ifdef PBL_COLOR
    window_set_background_color(my_window, pattern < PATTERN_BW_TRESHOLD? GColorBlack : GColorOxfordBlue);
  #else 
    window_set_background_color(my_window, GColorBlack);
  #endif  
}


// handle configuration change
static void in_recv_handler(DictionaryIterator *iterator, void *context) {
  Tuple *t = dict_read_first(iterator);

  while (t)  {

    switch(t->key)    {
    
      case KEY_PATTERN:
           persist_write_int(KEY_PATTERN, t->value->uint8);
           current_pattern = t->value->uint8;
           change_pattern(current_pattern);
           layer_mark_dirty(effect_layer_get_layer(effect_layer));
           break;
    }    
    
    t = dict_read_next(iterator);
  }
  
}  

static void battery_handler(BatteryChargeState state) {
   
   #ifdef PBL_COLOR
     switch (state.charge_percent) {
       case 100: bitmap_layer_set_background_color(battery_layer, GColorBrightGreen); break;
       case 90: bitmap_layer_set_background_color(battery_layer, GColorGreen); break;
       case 80: bitmap_layer_set_background_color(battery_layer, GColorIslamicGreen); break;
       case 70: bitmap_layer_set_background_color(battery_layer, GColorKellyGreen); break;
       case 60: bitmap_layer_set_background_color(battery_layer, GColorBrass); break;
       case 50: bitmap_layer_set_background_color(battery_layer, GColorLimerick); break;
       case 40: bitmap_layer_set_background_color(battery_layer, GColorYellow); break;
       case 30: bitmap_layer_set_background_color(battery_layer, GColorIcterine); break;
       case 20: bitmap_layer_set_background_color(battery_layer, GColorRajah); break;
       case 10: bitmap_layer_set_background_color(battery_layer, GColorRed); break;
   }
     
   #endif
  
   APP_LOG(APP_LOG_LEVEL_DEBUG,"Percent = %d, X = %d, W = %d", state.charge_percent, 72 - 72*state.charge_percent/100, 144*state.charge_percent/100);  
  
   layer_set_frame(bitmap_layer_get_layer(battery_layer), GRect(72 - 72*state.charge_percent/100, 167,  144*state.charge_percent/100, 168));
}


TextLayer* create_datetime_layer(GRect coords, int font) {
  TextLayer *text_layer = text_layer_create(coords);
  text_layer_set_font(text_layer, fonts_load_custom_font(resource_get_handle(font)));
  text_layer_set_text_color(text_layer, GColorWhite);  
  text_layer_set_background_color(text_layer, GColorClear);  
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(my_window), text_layer_get_layer(text_layer));
  return text_layer;
}



void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  
   if (!clock_is_24h_style()) {
    
        if( tick_time->tm_hour > 11 ) {   // YG Jun-25-2014: 0..11 - am 12..23 - pm
            if( tick_time->tm_hour > 12 ) tick_time->tm_hour -= 12;
        } else {
            if( tick_time->tm_hour == 0 ) tick_time->tm_hour = 12;
        }        
    }
  
    if (units_changed & MINUTE_UNIT) { // on minutes change - change time
      strftime(s_time, sizeof(s_time), "%H:%M", tick_time);
      text_layer_set_text(text_time, s_time);
    }  
    
    if (units_changed & DAY_UNIT) { // on day change - change date
      strftime(s_date, sizeof(s_date), "%b%d", tick_time);
      text_layer_set_text(text_date, s_date);
    
      strftime(s_dow, sizeof(s_dow), "%a", tick_time);
      text_layer_set_text(text_dow, s_dow);
    }
  
}

void handle_init(void) {
  my_window = window_create();
  
  #ifdef PBL_COLOR
    window_set_background_color(my_window, GColorOxfordBlue);
  #else 
    window_set_background_color(my_window, GColorBlack);
  #endif  
  window_stack_push(my_window, true);
  
  text_date = create_datetime_layer(GRect(0,6,144,52), RESOURCE_ID_MOLOT_46);
  text_time = create_datetime_layer(GRect(0,52,144,52), RESOURCE_ID_MOLOT_52);
  text_dow = create_datetime_layer(GRect(0,106,144,52), RESOURCE_ID_MOLOT_46);
  
  battery_layer = bitmap_layer_create(GRect(0,167,144,168));
  #ifndef PBL_COLOR
    bitmap_layer_set_background_color(battery_layer, GColorWhite);
  #else 
    bitmap_layer_set_background_color(battery_layer, GColorBrightGreen);
  #endif  
  layer_add_child(window_get_root_layer(my_window), bitmap_layer_get_layer(battery_layer));
  
    
  // ** { begin setup mask for MASK effect
  mask.text = NULL;
  mask.bitmap_mask = NULL;
  mask.mask_color = GColorWhite;
  mask.background_color = GColorClear;
  
  // reading and setting background pattern
  current_pattern = persist_read_int(KEY_PATTERN)? persist_read_int(KEY_PATTERN) : 0;
  current_pattern = 3;
  change_pattern(current_pattern);
  
  // ** end setup mask }

  //creating effect layer
  effect_layer = effect_layer_create(GRect(0,0,144,167));
  effect_layer_add_effect(effect_layer, effect_mask, &mask);
  layer_add_child(window_get_root_layer(my_window), effect_layer_get_layer(effect_layer));
  
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
   //Get a time structure so that the face doesn't start blank
  time_t temp = time(NULL);
  struct tm *t = localtime(&temp);
 
  //Manually call the tick handler when the window is loading
  tick_handler(t, DAY_UNIT | MINUTE_UNIT);
  
  //getting battery info
  battery_state_service_subscribe(battery_handler);
  battery_handler(battery_state_service_peek());
  
  // subscribing to JS messages
  app_message_register_inbox_received(in_recv_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
    
}

void handle_deinit(void) {
  
  //clearning MASK
  gbitmap_destroy(mask.bitmap_background);
  effect_layer_destroy(effect_layer);
  text_layer_destroy(text_date);
  text_layer_destroy(text_time);
  text_layer_destroy(text_dow);
  
  window_destroy(my_window);
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  
  app_message_deregister_callbacks();
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
