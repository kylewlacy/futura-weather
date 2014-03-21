#ifndef futura_weather_disconnected_layer_h
#define futura_weather_disconnected_layer_h

#include "pebble.h"
#include "ui_state.h"
#include "config.h"

typedef struct Watchface Watchface;

typedef struct DisconnectedLayer DisconnectedLayer;

DisconnectedLayer* disconnected_layer_create(Watchface* watchface);
void disconnected_layer_destroy(DisconnectedLayer* disconnected_layer);

Watchface* disconnected_layer_get_watchface(
    DisconnectedLayer* disconnected_layer
);
Layer* disconnected_layer_get_layer(
    DisconnectedLayer* disconnected_layer
);
Layer* disconnected_layer_get_root_layer(
    DisconnectedLayer* disconnected_layer
);

void disconnected_layer_set_visible(
    DisconnectedLayer* disconnected_layer, bool visible
);
Animation* disconnected_layer_create_animation(
    DisconnectedLayer* disconnected_layer, bool visible
);

#endif
