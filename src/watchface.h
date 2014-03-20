#ifndef futura_weather_watchface_h
#define futura_weather_watchface_h

#include "pebble.h"
#include "preferences.h"
#include "font_collection.h"
#include "ui_state.h"

#include "layers/statusbar_layer.h"
#include "layers/date_time_layer.h"
#include "layers/weather_layer.h"

struct Watchface {
    Layer* layer;
    FontCollection* fonts;
    UIState* ui_state;
    
    StatusbarLayer* statusbar_layer;
    DateTimeLayer* date_time_layer;
    WeatherLayer* weather_layer;
};
typedef struct Watchface Watchface;

Watchface* watchface_create(Window* window, UIState initial_ui_state);
void watchface_destroy(Watchface* watchface);

void watchface_set_statusbar_visible(
    Watchface* watchface, bool visible, bool animate
);
void watchface_set_weather_visible(
    Watchface* watchface, bool visible, bool animate
);

#endif
