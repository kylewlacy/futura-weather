#ifndef futura_weather_weather_layer_h
#define futura_weather_weather_layer_h

#include "pebble.h"
#include "ui_state.h"
#include "config.h"

struct Watchface;

typedef struct {
    struct Watchface* watchface;
    
    Layer* layer;
    
    BitmapLayer* conditions_layer;
    TextLayer* temperature_layer;
    
    GBitmap* conditions_icon;
    uint32_t conditions_resource;
    
#ifdef LIGHT_WEATHER
    InverterLayer* inverter_layer;
#endif
} WeatherLayer;

WeatherLayer* weather_layer_create(struct Watchface* watchface);
void weather_layer_destroy(WeatherLayer* weather_layer);

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
