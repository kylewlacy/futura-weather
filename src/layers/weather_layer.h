#ifndef futura_weather_weather_layer_h
#define futura_weather_weather_layer_h

#include "pebble.h"
#include "ui_state.h"
#include "config.h"

typedef struct Watchface Watchface;

typedef struct WeatherLayer WeatherLayer;

WeatherLayer* weather_layer_create(Watchface* watchface);
void weather_layer_destroy(WeatherLayer* weather_layer);

Watchface* weather_layer_get_watchface(
    WeatherLayer* weather_layer
);
Layer* weather_layer_get_layer(
    WeatherLayer* weather_layer
);
Layer* weather_layer_get_root_layer(
    WeatherLayer* weather_layer
);

void weather_layer_set_visible(
    WeatherLayer* weather_layer, bool visible
);
Animation* weather_layer_create_animation(
    WeatherLayer* weather_layer, bool visible
);

void weather_layer_set_temperature(
    WeatherLayer* weather_layer, int temperature
);
void weather_layer_set_conditions(
    WeatherLayer* weather_layer, WeatherConditions conditions
);

#endif
