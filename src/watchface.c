#include <pebble.h>
#include "watchface.h"
#include "wfLib.h"

static bool dp;
static const int posmax=30;
static GPoint pos[30];
static GPoint dsp[10];

static void scramble(){
  GPoint npos[30];
  memcpy(npos,pos,sizeof(npos));
  int max=30;
  int po=0;
  while (po<30){
    int sel=rand()%max;
    pos[po++]=npos[sel];
    for (int i=sel;i<max-1;i++)
      npos[i]=npos[i+1];
    max--;
  }
}

void watchfaceInit(){
  for (int i=0;i<posmax;i++){
    pos[i].x=((i%6)*18)+4;
    pos[i].y=(i/6)*15;
  }
  scramble();
  memset(dsp,0,sizeof(dsp));
  loadBitmapFromResource(0,RESOURCE_ID_d);
  loadBitmapFromResource(1,RESOURCE_ID_e1a1);
  loadBitmapFromResource(2,RESOURCE_ID_e1a2);
  createTextLayer(0,GRect(0, 123, 144, 45),fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT),GTextAlignmentCenter,GColorClear,GColorBlack);
}

static void shoot(int x, int y){
  for (int i=0;i<10;i++)
    if (dsp[i].x==0){
      dsp[i].x=x;
      dsp[i].y=y;
      return;
    }
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    // Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }
  //if (dp)
  //  buffer[2]=' ';
  dp=!dp;
  // Display this time on the TextLayer
  setTextLayer(0, buffer);
}

void updateWatchfaceLayer(GContext* ctx) {
  // We make sure the dimensions of the GRect to draw into
  // are equal to the size of the bitmap--otherwise the image
  // will automatically tile. Which might be what *you* want.
  
  int e1=dp?1:2;
  int sec=time(NULL)%60;
  if (sec==60){
    scramble();
    return;
  }
  int sec2=sec/2;
  int secm6=sec%6;
  int xx=secm6*6;
  int yy=(sec/6)*6;
  if ((sec/6)%2)
    xx=33-xx;
  for (int i=0;i<posmax-sec2;i++)
    draw(ctx,e1,xx+pos[i].x,yy+pos[i].y);
  int shooting=rand()%60;
  if (shooting<posmax-sec2)
    shoot(pos[shooting].x+xx+3,pos[shooting].y+yy);
  for (int i=0;i<10;i++){
    if (dsp[i].x==0)
      continue;
    dsp[i].y+=10;
    if (dsp[i].y>150)
      dsp[i].x=0;
    else
      draw(ctx,0,dsp[i].x,dsp[i].y);
  }
}

void timerHandler(struct tm *tick_time, TimeUnits units_changed){
  update_time();
}