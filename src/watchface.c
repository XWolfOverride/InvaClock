#include <pebble.h>
#include "watchface.h"
#include "wfLib.h"

static bool dp; //Odd second handler
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
  loadBitmapFromResource(3,RESOURCE_ID_edie);
#ifdef PBL_PLATFORM_CHALK
  createTextLayer(0,GRect(40, 130, 100, 45),fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS),GTextAlignmentCenter,GColorIslamicGreen,GColorBlack);
  createTextLayer(1,GRect(50, 2, 80, 15),fonts_get_system_font(FONT_KEY_GOTHIC_14),GTextAlignmentCenter,GColorWhite,GColorBlack);
  createTextLayer(2,GRect(50, 120, 80, 15),fonts_get_system_font(FONT_KEY_GOTHIC_14),GTextAlignmentCenter,GColorWhite,GColorBlack);
#else  
 #ifdef PBL_COLOR  
  createTextLayer(0,GRect(-3, 123, 144, 45),fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT),GTextAlignmentLeft,GColorIslamicGreen,GColorBlack);
 #else
  createTextLayer(0,GRect(-3, 123, 144, 45),fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT),GTextAlignmentLeft,GColorWhite,GColorBlack);
 #endif
  createTextLayer(2,GRect(111, 151, 32, 15),fonts_get_system_font(FONT_KEY_GOTHIC_14),GTextAlignmentRight,GColorWhite,GColorBlack);
  createTextLayer(1,GRect(111, 139, 33, 15),fonts_get_system_font(FONT_KEY_GOTHIC_14),GTextAlignmentRight,GColorWhite,GColorBlack);
#endif
  setlocale(LC_ALL, "");
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
  static char dbuffer[] = "88-00";
  static char wbuffer[] = "                        ";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    // Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }
  strftime(dbuffer, sizeof(dbuffer), "%d-%m", tick_time);
  strftime(wbuffer, sizeof(wbuffer), "%a", tick_time);
//  if (dp)
//    buffer[2]=' ';
  dp=!dp;

  // Display this time on the TextLayer
  setTextLayer(0, buffer);
  setTextLayer(1, wbuffer);
  setTextLayer(2, dbuffer);
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
#ifdef PBL_PLATFORM_CHALK
  int sec2=sec/2;
  int secm6=sec%6;
  int xx=secm6*6;
  int yy=(sec/6)*3;
  int lastEnemy=posmax-sec2;
  if ((sec/6)%2)
    xx=33-xx;
  xx+=20;
  yy+=22;
#else  
  int sec2=sec/2;
  int secm6=sec%6;
  int xx=secm6*6;
  int yy=(sec/6)*6;
  int lastEnemy=posmax-sec2;
  if ((sec/6)%2)
    xx=33-xx;
#endif
  // enemy draw
  for (int i=0;i<lastEnemy;i++)
    draw(ctx,e1,xx+pos[i].x,yy+pos[i].y);
  if (sec2>0 && (sec%2==0))
    draw(ctx,3,xx+pos[lastEnemy].x,yy+pos[lastEnemy].y);
  // enemy shoot
  int shooting=rand()%60;
  if (shooting<posmax-sec2)
    shoot(pos[shooting].x+xx+3,pos[shooting].y+yy);
  // enemy shoot draw
  for (int i=0;i<10;i++){
    if (dsp[i].x==0)
      continue;
    dsp[i].y+=10;
    if (dsp[i].y>130)
      dsp[i].x=0;
    else
      draw(ctx,0,dsp[i].x,dsp[i].y);
  }
}

void timerHandler(struct tm *tick_time, TimeUnits units_changed){
  update_time();
}