#include <pebble.h>
#include "main.h"  
#include <pebble-effect-layer/pebble-effect-layer.h>

static Window *my_window;

static TextLayer *text_time, *text_date, *text_dow;
static EffectLayer* effect_layer;
static EffectMask mask;

static char s_date[] = "HELLO"; //test
static char s_time[] = "HOWRE"; //test
static char s_dow[] = "YOU"; //test

static int current_pattern; // currently loaded pattern
static GBitmap *background;
Layer *battery_layer;


// changes pattern of background
static void change_pattern(int pattern) {
  
  // destroying existing pattern
  if (background != NULL) {
    gbitmap_destroy(background);
    background = NULL;
  }
  
  // APP_LOG(APP_LOG_LEVEL_DEBUG, "1. Before: Pattern = %d, memory = %d", pattern, (int)heap_bytes_free());
  
  // and creating new one
  background = gbitmap_create_with_resource(pattern_array[pattern]);
  mask.bitmap_background = background;
  
  // APP_LOG(APP_LOG_LEVEL_DEBUG, "2. After: Pattern = %d, memory = %d", pattern, (int)heap_bytes_free());
  
  //for b&w patterns do a b&w background
  #ifdef PBL_COLOR
    window_set_background_color(my_window, pattern < PATTERN_BW_TRESHOLD? GColorBlack : GColorOxfordBlue);
  #else 
    window_set_background_color(my_window, GColorBlack);
  #endif  
}

static void battery_update(Layer *layer, GContext *ctx) {
   GColor color = GColorWhite;
   int percent = battery_state_service_peek().charge_percent;
   GRect bounds = layer_get_bounds(layer);
  
   #ifdef PBL_COLOR
     switch (percent) {
       case 100: color = GColorBrightGreen; break;
       case 90: color = GColorGreen; break;
       case 80: color = GColorIslamicGreen; break;
       case 70: color = GColorKellyGreen; break;
       case 60: color = GColorBrass; break;
       case 50: color = GColorLimerick; break;
       case 40: color = GColorYellow; break;
       case 30: color = GColorIcterine; break;
       case 20: color = GColorRajah; break;
       case 10: color = GColorRed; break;
   }
   #endif
  
   //PP_LOG(APP_LOG_LEVEL_DEBUG,"Percent = %d, X = %d, W = %d", state.charge_percent, 72 - 72*state.charge_percent/100, 144*state.charge_percent/100);  

   graphics_context_set_fill_color(ctx, color);  
     
   #ifdef PBL_ROUND  
      graphics_fill_radial(ctx, bounds, GOvalScaleModeFitCircle, 4, DEG_TO_TRIGANGLE(180 - 180*percent/100), DEG_TO_TRIGANGLE(180 + 180*percent/100));
   #else
      graphics_fill_rect(ctx, GRect(bounds.size.w/2 - (bounds.size.w/2)*percent/100, bounds.size.h - 2 , bounds.size.w*percent/100, 2), 0, GCornerNone) ;
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
  layer_mark_dirty(battery_layer);  
}


TextLayer* create_datetime_layer(GRect coords, int font) {
  TextLayer *text_layer = text_layer_create(coords);
  text_layer_set_font(text_layer, fonts_load_custom_font(resource_get_handle(font)));
  text_layer_set_text_color(text_layer, GColorWhite);  
  text_layer_set_background_color(text_layer, GColorClear);  
  text_layer_set_overflow_mode(text_layer, GTextOverflowModeFill);
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
  
  Layer *window_layer = window_get_root_layer(my_window);
  GRect bounds = layer_get_bounds(window_layer);
  
  #ifndef PBL_ROUND
    text_date = create_datetime_layer(GRect(0,6,bounds.size.w,52), RESOURCE_ID_MOLOT_46);
    text_time = create_datetime_layer(GRect(0,52,bounds.size.w,52), RESOURCE_ID_MOLOT_52);
    text_dow = create_datetime_layer(GRect(0,106,bounds.size.w,52), RESOURCE_ID_MOLOT_46);
  #else
    text_date = create_datetime_layer(GRect(0,19,bounds.size.w,52), RESOURCE_ID_MOLOT_46);
    text_time = create_datetime_layer(GRect(0,62,bounds.size.w,52), RESOURCE_ID_MOLOT_52);
    text_dow = create_datetime_layer(GRect(0,113,bounds.size.w,52), RESOURCE_ID_MOLOT_46);
  #endif
  
  battery_layer = layer_create(bounds);
  layer_set_update_proc(battery_layer, battery_update);
  layer_add_child(window_get_root_layer(my_window), battery_layer);
    
  // ** { begin setup mask for MASK effect
  mask.text = NULL;
  mask.bitmap_mask = NULL;
  
  mask.mask_colors = malloc(sizeof(GColor)*2);
  mask.mask_colors[0] = GColorWhite;
  mask.mask_colors[1] = GColorClear;
  
  mask.background_color = GColorClear;
  
  // reading and setting background pattern
  current_pattern = persist_read_int(KEY_PATTERN)? persist_read_int(KEY_PATTERN) : 0;
  change_pattern(current_pattern);
  
  // ** end setup mask }

  //creating effect layer
  effect_layer = effect_layer_create(GRect((bounds.size.w-BACKGROUND_W)/2, (bounds.size.h-BACKGROUND_H)/2, BACKGROUND_W, BACKGROUND_H));
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
  app_message_open(APP_MESSAGE_INBOX_SIZE_MINIMUM, APP_MESSAGE_OUTBOX_SIZE_MINIMUM);
    
}

void handle_deinit(void) {
  
  //clearning MASK
  gbitmap_destroy(background);
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
