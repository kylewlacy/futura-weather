#ifndef futura_weather_date_time_layer_h
#define futura_weather_date_time_layer_h

#include "pebble.h"
#include "ui_state.h"

struct Watchface;

typedef struct {
    struct Watchface* watchface;
    Layer* layer;
    TextLayer* time_layer;
    TextLayer* date_layer;
} DateTimeLayer;

DateTimeLayer* date_time_layer_create(struct Watchface* watchface);
void date_time_layer_destroy(DateTimeLayer* date_time_layer);

void date_time_layer_update_frame(
    DateTimeLayer* date_time_layer
);
Animation* date_time_layer_create_time_animation(
    DateTimeLayer* date_time_layer
);
Animation* date_time_layer_create_date_animation(
    DateTimeLayer* date_time_layer
);

#endif
