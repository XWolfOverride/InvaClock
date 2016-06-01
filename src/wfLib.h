#pragma once
#include "pebble.h"

// CONFIGURATION --------------------

#define _CFG_maxBitmaps 10
#ifdef PBL_COLOR
#define _CFG_backgroundColor GColorBlack
#else
#define _CFG_backgroundColor GColorBlack
#endif
#define _CFG_maxTextLayers 3
#define _CFG_timerInterval SECOND_UNIT

// END CONFIGURAITION ---------------

typedef void(* wflibProc)();
  
//-- INIT
void initWF(LayerUpdateProc proc_update_layer,TickHandler tick_handler,wflibProc initProc,wflibProc deinitProc);
void deinitWF();

//-- MISC
void redraw();
void setBackground(GColor backColor);

//-- BITMAP
void loadBitmapFromResource(int bmpId,int resource);
void unloadBitmap(int position);
void draw(GContext *ctx, int bmpId, int x, int y);

//-- TEXTs
void createTextLayer(int layer, GRect rect, GFont font, GTextAlignment alignment, GColor color, GColor backColor);
void disposeTextLayer(int layer);
void setTextLayer(int layer,const char *text);