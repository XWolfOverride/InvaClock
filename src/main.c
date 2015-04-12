#include <pebble.h>
#include "wfLib.h"
#include "watchface.h"
static Window *s_main_window;
static Layer *layer;

static void layer_update_callback(Layer *me, GContext* ctx) {
  // We make sure the dimensions of the GRect to draw into
  // are equal to the size of the bitmap--otherwise the image
  // will automatically tile. Which might be what *you* want.

 updateWatchfaceLayer(ctx);
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  layer = layer_create(bounds);
  layer_set_update_proc(layer, layer_update_callback);
  layer_add_child(window_layer, layer);

  initWF(window,layer);
  watchfaceInit();
  timerHandler(NULL,0);
}

static void main_window_unload(Window *window) {
  deinitWF();
  layer_destroy(layer);
}

static void init() {
  srand(time(NULL));
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  window_set_background_color(s_main_window, backgroundColor);  
  
  if (timerInterval>0)
    tick_timer_service_subscribe(timerInterval, timerHandler);
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
