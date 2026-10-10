#include <pebble.h>
#include "main.h"
#include <pebble-effect-layer/pebble-effect-layer.h>

static Window *my_window;

static TextLayer *text_time, *text_date, *text_dow;
static GFont font_small, font_large;
static EffectLayer *effect_layer;
static EffectMask mask;
static GColor mask_colors[2];

static char s_date[] = "HELLO"; //test
static char s_time[] = "HOWRE"; //test
static char s_dow[] = "YOU"; //test

static int current_pattern; // currently loaded pattern
static GBitmap *background;
static Layer *battery_layer;


// maps pattern number to bitmap resource. On B&W watches color patterns fall back to their B&W twin
static uint32_t pattern_resource(int pattern) {
  #ifdef PBL_COLOR
    switch (pattern) {
      case 10: return RESOURCE_ID_HORIZONTAL_COLOR;
      case 11: return RESOURCE_ID_VERTICAL_COLOR;
      case 12: return RESOURCE_ID_TV_NOISE_COLOR;
      case 13: return RESOURCE_ID_SUNSET_COLOR;
    }
  #endif

  switch (pattern % PATTERN_BW_TRESHOLD) {
    case 1: return RESOURCE_ID_VERTICAL_BW;
    case 2: return RESOURCE_ID_TV_NOISE_BW;
    case 3: return RESOURCE_ID_GRID_BW;
    default: return RESOURCE_ID_HORIZONTAL_BW;
  }
}

// changes pattern of background
static void change_pattern(int pattern) {

  // destroying existing pattern
  if (background != NULL) {
    gbitmap_destroy(background);
    background = NULL;
  }

  // and creating new one
  background = gbitmap_create_with_resource(pattern_resource(pattern));
  mask.bitmap_background = background;

  //for b&w patterns do a b&w background
  window_set_background_color(my_window,
    PBL_IF_COLOR_ELSE(pattern < PATTERN_BW_TRESHOLD ? GColorBlack : GColorOxfordBlue, GColorBlack));
}

static GColor battery_color(int percent) {
  #ifdef PBL_COLOR
    if (percent >= 100) return GColorBrightGreen;
    if (percent >= 90) return GColorGreen;
    if (percent >= 80) return GColorIslamicGreen;
    if (percent >= 70) return GColorKellyGreen;
    if (percent >= 60) return GColorBrass;
    if (percent >= 50) return GColorLimerick;
    if (percent >= 40) return GColorYellow;
    if (percent >= 30) return GColorIcterine;
    if (percent >= 20) return GColorRajah;
    if (percent >= 10) return GColorRed;
  #endif
  return GColorWhite;
}

static void battery_update(Layer *layer, GContext *ctx) {
   int percent = battery_state_service_peek().charge_percent;

   graphics_context_set_fill_color(ctx, battery_color(percent));

   #ifdef PBL_ROUND
      GRect bounds = layer_get_bounds(layer);
      graphics_fill_radial(ctx, bounds, GOvalScaleModeFitCircle, RING_W, DEG_TO_TRIGANGLE(180 - 180*percent/100), DEG_TO_TRIGANGLE(180 + 180*percent/100));
   #else
      // keep the bar visible above Timeline Quick View
      GRect bounds = layer_get_unobstructed_bounds(layer);
      graphics_fill_rect(ctx, GRect(bounds.size.w/2 - (bounds.size.w/2)*percent/100, bounds.size.h - BATTERY_BAR_H, bounds.size.w*percent/100, BATTERY_BAR_H), 0, GCornerNone);
   #endif
}

// positions text rows and pattern area within visible (unobstructed) part of the screen.
// When Timeline Quick View covers part of the screen only the time is shown
static void layout(void) {
  Layer *window_layer = window_get_root_layer(my_window);
  GRect bounds = layer_get_bounds(window_layer);
  GRect visible = layer_get_unobstructed_bounds(window_layer);
  bool obstructed = visible.size.h < bounds.size.h;

  int16_t left = (bounds.size.w - BACKGROUND_W) / 2;
  GRect area;

  if (!obstructed) {
    area = GRect(left, (bounds.size.h - BACKGROUND_H) / 2, BACKGROUND_W, BACKGROUND_H);
  } else {
    // pattern area shrinks to just the time row, centered in what's left of the screen
    area = GRect(left, visible.origin.y + (visible.size.h - ROW_H) / 2, BACKGROUND_W, ROW_H);
  }

  layer_set_frame(text_layer_get_layer(text_date), GRect(0, area.origin.y + ROW_DATE_Y, bounds.size.w, ROW_H));
  layer_set_frame(text_layer_get_layer(text_time),
    GRect(0, area.origin.y + (obstructed ? 0 : ROW_TIME_Y), bounds.size.w, ROW_H));
  layer_set_frame(text_layer_get_layer(text_dow), GRect(0, area.origin.y + ROW_DOW_Y, bounds.size.w, ROW_H));
  layer_set_hidden(text_layer_get_layer(text_date), obstructed);
  layer_set_hidden(text_layer_get_layer(text_dow), obstructed);

  layer_set_frame(effect_layer_get_layer(effect_layer), area);
  layer_mark_dirty(battery_layer);
}

static void unobstructed_change(AnimationProgress progress, void *context) {
  layout();
}

static void unobstructed_did_change(void *context) {
  layout();
}


// handle configuration change
static void in_recv_handler(DictionaryIterator *iterator, void *context) {
  Tuple *t = dict_find(iterator, MESSAGE_KEY_PATTERN);

  if (t) {
    // Clay sends radio values as strings, older config pages sent numbers
    current_pattern = t->type == TUPLE_CSTRING ? atoi(t->value->cstring) : (int)t->value->int32;
    persist_write_int(PERSIST_KEY_PATTERN, current_pattern);
    change_pattern(current_pattern);
    layer_mark_dirty(effect_layer_get_layer(effect_layer));
  }
}

static void battery_handler(BatteryChargeState state) {
  layer_mark_dirty(battery_layer);
}

// need to catch when app resumes focus after notification, otherwise background won't restore
static void app_focus_changed(bool focused) {
  if (focused) {
    layer_mark_dirty(effect_layer_get_layer(effect_layer));
  }
}


static TextLayer* create_datetime_layer(GFont font) {
  TextLayer *text_layer = text_layer_create(GRectZero);
  text_layer_set_font(text_layer, font);
  text_layer_set_text_color(text_layer, GColorWhite);
  text_layer_set_background_color(text_layer, GColorClear);
  text_layer_set_overflow_mode(text_layer, GTextOverflowModeFill);
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(my_window), text_layer_get_layer(text_layer));
  return text_layer;
}


static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {

    if (units_changed & MINUTE_UNIT) { // on minutes change - change time
      strftime(s_time, sizeof(s_time), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);
      text_layer_set_text(text_time, s_time);
    }

    if (units_changed & DAY_UNIT) { // on day change - change date
      strftime(s_date, sizeof(s_date), "%b%d", tick_time);
      text_layer_set_text(text_date, s_date);

      strftime(s_dow, sizeof(s_dow), "%a", tick_time);
      text_layer_set_text(text_dow, s_dow);
    }

}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  font_small = fonts_load_custom_font(resource_get_handle(FONT_SMALL));
  font_large = fonts_load_custom_font(resource_get_handle(FONT_LARGE));

  text_date = create_datetime_layer(font_small);
  text_time = create_datetime_layer(font_large);
  text_dow = create_datetime_layer(font_small);

  battery_layer = layer_create(bounds);
  layer_set_update_proc(battery_layer, battery_update);
  layer_add_child(window_layer, battery_layer);

  // ** { begin setup mask for MASK effect
  mask.text = NULL;
  mask.bitmap_mask = NULL;

  mask_colors[0] = GColorWhite;
  mask_colors[1] = GColorClear;
  mask.mask_colors = mask_colors;

  mask.background_color = GColorClear;

  // reading and setting background pattern
  current_pattern = persist_exists(PERSIST_KEY_PATTERN) ? persist_read_int(PERSIST_KEY_PATTERN) : 0;
  change_pattern(current_pattern);

  // ** end setup mask }

  //creating effect layer
  effect_layer = effect_layer_create(GRect((bounds.size.w-BACKGROUND_W)/2, (bounds.size.h-BACKGROUND_H)/2, BACKGROUND_W, BACKGROUND_H));
  effect_layer_add_effect(effect_layer, effect_mask, &mask);
  layer_add_child(window_layer, effect_layer_get_layer(effect_layer));

  layout();
}

static void window_unload(Window *window) {
  //clearing MASK
  effect_layer_destroy(effect_layer);
  gbitmap_destroy(background);
  background = NULL;

  layer_destroy(battery_layer);
  text_layer_destroy(text_date);
  text_layer_destroy(text_time);
  text_layer_destroy(text_dow);
  fonts_unload_custom_font(font_small);
  fonts_unload_custom_font(font_large);
}

static void handle_init(void) {

  app_focus_service_subscribe_handlers((AppFocusHandlers){
    .did_focus = app_focus_changed
  });

  my_window = window_create();
  window_set_window_handlers(my_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload
  });
  window_stack_push(my_window, true);

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  //Get a time structure so that the face doesn't start blank
  time_t temp = time(NULL);
  tick_handler(localtime(&temp), DAY_UNIT | MINUTE_UNIT);

  //getting battery info
  battery_state_service_subscribe(battery_handler);

  // re-layout when Timeline Quick View appears / disappears
  unobstructed_area_service_subscribe((UnobstructedAreaHandlers) {
    .change = unobstructed_change,
    .did_change = unobstructed_did_change
  }, NULL);

  // subscribing to JS messages
  app_message_register_inbox_received(in_recv_handler);
  app_message_open(64, 0);
}

static void handle_deinit(void) {
  app_focus_service_unsubscribe();
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  unobstructed_area_service_unsubscribe();
  app_message_deregister_callbacks();

  window_destroy(my_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
