#ifndef futura_weather_statusbar_layer_h
#define futura_weather_statusbar_layer_h

#include "pebble.h"
#include "weather.h"
#include "ui_state.h"

struct Watchface;

typedef struct {
    struct Watchface* watchface;
    Layer* layer;
    
    BitmapLayer* battery_icon_layer;
    GBitmap* battery_icon;
    uint32_t battery_resource;
} StatusbarLayer;

StatusbarLayer* statusbar_layer_create(struct Watchface* watchface);
void statusbar_layer_destroy(StatusbarLayer* statusbar_layer);


Animation* statusbar_layer_create_animation(
    StatusbarLayer* statusbar_layer,
    bool visible
);
void statusbar_layer_set_visible(
    StatusbarLayer* statusbar_layer, bool visible
);

void statusbar_layer_handle_battery(
    StatusbarLayer* statusbar_layer, BatteryChargeState battery
);

#endif
