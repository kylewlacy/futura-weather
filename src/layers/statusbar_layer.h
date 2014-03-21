#ifndef futura_weather_statusbar_layer_h
#define futura_weather_statusbar_layer_h

#include "pebble.h"
#include "weather.h"
#include "ui_state.h"

typedef struct Watchface Watchface;

typedef struct StatusbarLayer StatusbarLayer;

StatusbarLayer* statusbar_layer_create(Watchface* watchface);
void statusbar_layer_destroy(StatusbarLayer* statusbar_layer);

Watchface* statusbar_layer_get_watchface(
    StatusbarLayer* statusbar_layer
);
Layer* statusbar_layer_get_layer(
    StatusbarLayer* statusbar_layer
);
Layer* statusbar_layer_get_root_layer(
    StatusbarLayer* statusbar_layer
);

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
