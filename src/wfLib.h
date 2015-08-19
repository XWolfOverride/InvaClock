#pragma once
#include <pebble.h>

// CONFIGURATION --------------------
#define maxBitmaps 10
#define backgroundColor GColorBlack
#define maxTextLayers 3
#define timerInterval SECOND_UNIT

// END CONFIGURAITION ---------------

//-- INIT
void initWF(Window *window,Layer *layer);
void deinitWF();

//-- MISC
void redraw();
void setBackground(GColor backgroundColor);

//-- BITMAP
void loadBitmapFromResource(int bmpId,int resource);
void unloadBitmap(int position);
void draw(GContext* ctx,int bmpId,int x, int y);

//-- TEXTs
void createTextLayer(int layer,GRect rect,GFont font,GTextAlignment alignment,GColor color,GColor backgroundColor);
void disposeTextLayer(int layer);
void setTextLayer(int layer,const char* text);