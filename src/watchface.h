#pragma once

void watchfaceInit();
void watchfaceDestroy();

void updateWatchfaceLayer(GContext* ctx);
void timerHandler(struct tm *tick_time, TimeUnits units_changed);