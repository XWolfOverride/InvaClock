#include <pebble.h>
#include "wfLib.h"

static GBitmap *bitmap[_CFG_maxBitmaps];
//static int bitmapCount=0;
static TextLayer *textLayers[_CFG_maxTextLayers];
static Window *mainWindow;
static Layer *mainLayer;

void initWF(Window *window,Layer *layer){
  mainWindow=window;
  mainLayer=layer;
  for (int i=0;i<_CFG_maxBitmaps;i++){
    bitmap[i]=NULL;
  }
}

void deinitWF(){
  for (int i=0;i<_CFG_maxBitmaps;i++){
    if (bitmap[i]==NULL)
      gbitmap_destroy(bitmap[i]);
  }
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
  text_layer_set_background_color(tlayer, _CFG_backgroundColor);
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