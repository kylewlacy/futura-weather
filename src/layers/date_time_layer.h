#ifndef futura_weather_date_time_layer_h
#define futura_weather_date_time_layer_h

#include "pebble.h"
#include "ui_state.h"

typedef struct Watchface Watchface;

typedef struct DateTimeLayer DateTimeLayer;

DateTimeLayer* date_time_layer_create(Watchface* watchface);
void date_time_layer_destroy(DateTimeLayer* date_time_layer);

void date_time_layer_update_date(
    DateTimeLayer* date_time_layer,
    struct tm* now, char* translation
);
void date_time_layer_update_time(
    DateTimeLayer* date_time_layer,
    struct tm* now, bool is_24h
);

Watchface* date_time_layer_get_watchface(
    DateTimeLayer* date_time_layer
);
Layer* date_time_layer_get_layer(
    DateTimeLayer* date_time_layer
);
Layer* date_time_layer_get_root_layer(
    DateTimeLayer* date_time_layer
);

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
