#include <pebble.h>
static Window *s_main_window;
static TextLayer *s_time_layer;
static Layer *layer;
static GBitmap *e1a1,*e1a2,*d;
static bool dp;
static const int posmax=30;
static GPoint pos[30];
static GPoint dsp[10];

static void draw(GContext* ctx,GBitmap *image,int x, int y){
  GRect bounds = image->bounds;  
  graphics_draw_bitmap_in_rect(ctx, image, (GRect) { .origin = { x, y }, .size = bounds.size });
}

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

static void shoot(int x, int y){
  for (int i=0;i<10;i++)
    if (dsp[i].x==0){
      dsp[i].x=x;
      dsp[i].y=y;
      return;
    }
}

static void layer_update_callback(Layer *me, GContext* ctx) {
  // We make sure the dimensions of the GRect to draw into
  // are equal to the size of the bitmap--otherwise the image
  // will automatically tile. Which might be what *you* want.

  static GBitmap *e1;
  e1=dp?e1a1:e1a2;
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
      draw(ctx,d,dsp[i].x,dsp[i].y);
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
  text_layer_set_text(s_time_layer, buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void main_window_load(Window *window) {
  // Create time TextLayer
  //s_time_layer = text_layer_create(GRect(75, 130, 60, 28));
  s_time_layer = text_layer_create(GRect(0, 123, 144, 45));
  text_layer_set_background_color(s_time_layer, GColorBlack);
  text_layer_set_text_color(s_time_layer, GColorClear);
  
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  layer = layer_create(bounds);
  layer_set_update_proc(layer, layer_update_callback);
  layer_add_child(window_layer, layer);
  
  // Loading Images
  e1a1 = gbitmap_create_with_resource(RESOURCE_ID_e1a1);
  e1a2 = gbitmap_create_with_resource(RESOURCE_ID_e1a2);
  d = gbitmap_create_with_resource(RESOURCE_ID_d);

  // Improve the layout to be more like a watchface
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  
  update_time();
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  layer_destroy(layer);
  gbitmap_destroy(e1a1);
  gbitmap_destroy(e1a2);
  gbitmap_destroy(d);
}

static void init() {
  for (int i=0;i<posmax;i++){
    pos[i].x=((i%6)*18)+4;
    pos[i].y=(i/6)*15;
  }
  scramble();
  memset(dsp,0,sizeof(dsp));
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
  window_set_background_color(s_main_window, GColorBlack);  
  
  // Register with TickTimerService
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
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