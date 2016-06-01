#include <pebble.h>
#include "wfLib.h"

static bool dp; //Odd second handler
static const int posmax=30;
static GPoint pos[30];
static GPoint dsp[10];
static bool bat_charging;
static uint8_t bat_level;

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

static void handle_battery(BatteryChargeState charge_state) {
  bat_charging=charge_state.is_charging;
  bat_level=((charge_state.charge_percent*6)/100)+1;
  if (bat_level>6)
    bat_level=6;
  redraw();
}

static void update_time(bool force) {
  // Update odd second counter
  dp=!dp;
  // Get a tm structure
  time_t temp = time(NULL);
  long sec = time(NULL);
  // Abort in middle of a minute
  if (!force && (sec%60)!=0){
    redraw();
    return;
  }
  // Local time
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

  // Display this time on the TextLayer
  setTextLayer(0, buffer);
  setTextLayer(1, wbuffer);
  setTextLayer(2, dbuffer);
}

void watchfaceDestroy(){
  battery_state_service_unsubscribe();
//  connection_service_unsubscribe();
}

static void shoot(int x, int y){
  for (int i=0;i<10;i++)
    if (dsp[i].x==0){
      dsp[i].x=x;
      dsp[i].y=y;
      return;
    }
}


void timerHandler(struct tm *tick_time, TimeUnits units_changed){
  update_time(false);
}

void watchfaceInit(){
  battery_state_service_subscribe(handle_battery);
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
  loadBitmapFromResource(4,RESOURCE_ID_live);
#ifdef PBL_PLATFORM_CHALK
  createTextLayer(0,GRect(40, 130, 100, 45),fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS),GTextAlignmentCenter,GColorGreen,GColorBlack);
  createTextLayer(1,GRect(50, 2, 80, 15),fonts_get_system_font(FONT_KEY_GOTHIC_14),GTextAlignmentCenter,GColorWhite,GColorBlack);
  createTextLayer(2,GRect(50, 120, 80, 15),fonts_get_system_font(FONT_KEY_GOTHIC_14),GTextAlignmentCenter,GColorWhite,GColorBlack);
#else  
 #ifdef PBL_COLOR  
  createTextLayer(0,GRect(-3, 123, 114, 45),fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT),GTextAlignmentLeft,GColorGreen,GColorBlack);
 #else
  createTextLayer(0,GRect(-3, 123, 114, 45),fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT),GTextAlignmentLeft,GColorWhite,GColorBlack);
 #endif
  createTextLayer(2,GRect(111, 151, 32, 15),fonts_get_system_font(FONT_KEY_GOTHIC_14),GTextAlignmentRight,GColorWhite,GColorBlack);
  createTextLayer(1,GRect(111, 139, 33, 15),fonts_get_system_font(FONT_KEY_GOTHIC_14),GTextAlignmentRight,GColorWhite,GColorBlack);
#endif
  setlocale(LC_ALL, "");
  handle_battery(battery_state_service_peek());
  update_time(true);
}

void updateWatchfaceLayer(GContext* ctx) {
  // We make sure the dimensions of the GRect to draw into
  // are equal to the size of the bitmap--otherwise the image
  // will automatically tile. Which might be what *you* want.
  
  int e1=dp?1:2;
  int sec=time(NULL)%60;
  if (sec==0)
    scramble();
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
  // battery live indicator
#ifdef PBL_PLATFORM_CHALK
  int j;
  for (j=0;j<bat_level-1;j+=2)
    draw(ctx,4,165,77+(j*5));
  if (bat_level%2==1 && dp)
    draw(ctx,4,165,77+(j*5));
#else
  int j;
  for (j=0;j<bat_level-1;j+=2)
    draw(ctx,4,116+(j*5),134);
  if (bat_level%2==1 && dp)
    draw(ctx,4,116+(j*5),134);
#endif
}

static void layer_update_callback(Layer *me, GContext* ctx) {
  // We make sure the dimensions of the GRect to draw into
  // are equal to the size of the bitmap--otherwise the image
  // will automatically tile. Which might be what *you* want.

 updateWatchfaceLayer(ctx);
}

int main(void) {
  initWF(layer_update_callback,timerHandler,watchfaceInit,watchfaceDestroy);
  app_event_loop();
  deinitWF();
}
