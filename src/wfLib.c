#include <pebble.h>
#include "wfLib.h"

static GBitmap *bitmap[_CFG_maxBitmaps];
//static int bitmapCount=0;
static TextLayer *textLayers[_CFG_maxTextLayers];
static Window *mainWindow;
static Layer *mainLayer;
static LayerUpdateProc _proc_update_layer;
static TickHandler _tick_handler;
static wflibProc _initProc;
static wflibProc _deinitProc;

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  mainLayer = layer_create(bounds);
  layer_set_update_proc(mainLayer, _proc_update_layer);
  layer_add_child(window_layer, mainLayer);

  for (int i=0;i<_CFG_maxBitmaps;i++){
    bitmap[i]=NULL;
  }
  (*_initProc)();
  (*_tick_handler)(NULL,0);
}

static void main_window_unload(Window *window) {
  (*_deinitProc)();
  for (int i=0;i<_CFG_maxBitmaps;i++){
    if (bitmap[i]==NULL)
      gbitmap_destroy(bitmap[i]);
  }
  layer_destroy(mainLayer);
}

void initWF(LayerUpdateProc proc_update_layer,TickHandler tick_handler,wflibProc initProc,wflibProc deinitProc) {
  srand(time(NULL));
  // Register event handlers
  _proc_update_layer = proc_update_layer;
  _tick_handler = _tick_handler;
  _initProc = initProc;
  _deinitProc = deinitProc;
  // Create main Window element and assign to pointer
  mainWindow = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(mainWindow, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(mainWindow, true);
  window_set_background_color(mainWindow, _CFG_backgroundColor);  
  
  if (_CFG_timerInterval>0)
    tick_timer_service_subscribe(_CFG_timerInterval, _tick_handler);
}

void deinitWF() {
  // Destroy Window
  window_destroy(mainWindow);
  if (_CFG_timerInterval>0)
    tick_timer_service_unsubscribe();
}

void redraw(){
  layer_mark_dirty(mainLayer);
}

void setBackground(GColor backColor){
  window_set_background_color(mainWindow,_CFG_backgroundColor);
}

void loadBitmapFromResource(int bmpId,int resource){
  if (bmpId>=_CFG_maxBitmaps)
    return;
  unloadBitmap(bmpId);
  bitmap[bmpId]=gbitmap_create_with_resource(resource);
}

void unloadBitmap(int bmpId){
  if (bmpId>=_CFG_maxBitmaps)
    return;
  if (bitmap[bmpId]!=NULL){
    gbitmap_destroy(bitmap[bmpId]);
    bitmap[bmpId]=NULL;
  }
}

void draw(GContext *ctx, int bmpId, int x, int y){
  if (bmpId>=_CFG_maxBitmaps)
    return;
  GBitmap *image=bitmap[bmpId];
  GRect bounds = gbitmap_get_bounds(image);  
  graphics_draw_bitmap_in_rect(ctx, image, (GRect) { .origin = { x, y }, .size = bounds.size });
}

void createTextLayer(int layer, GRect rect, GFont font, GTextAlignment alignment, GColor color, GColor backColor){
  if (layer>_CFG_maxTextLayers)
    return;
  if (textLayers[layer]!=NULL)
    disposeTextLayer(layer);
  TextLayer *tlayer=text_layer_create(rect);
  text_layer_set_text_color(tlayer, color);
  text_layer_set_background_color(tlayer, backColor);
  text_layer_set_font(tlayer, font);
  text_layer_set_text_alignment(tlayer, alignment);
  layer_add_child(window_get_root_layer(mainWindow), text_layer_get_layer(tlayer));
  textLayers[layer]=tlayer;
}

void disposeTextLayer(int layer){
  if (layer>_CFG_maxTextLayers)
    return;
  if (textLayers[layer]!=NULL){
    text_layer_destroy(textLayers[layer]);
    textLayers[layer]=NULL;
  }
}

void setTextLayer(int layer,const char* text){
  if (layer>_CFG_maxTextLayers)
    return;
  text_layer_set_text(textLayers[layer], text);
}