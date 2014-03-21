#ifndef futura_weather_watchface_h
#define futura_weather_watchface_h

#include "pebble.h"
#include "preferences.h"
#include "font_collection.h"
#include "ui_state.h"

#include "layers/statusbar_layer.h"
#include "layers/date_time_layer.h"
#include "layers/weather_layer.h"

typedef struct Watchface Watchface;

Watchface* watchface_create(Window* window, UIState initial_ui_state);
void watchface_destroy(Watchface* watchface);

Layer* watchface_get_layer(Watchface* watchface);
Layer* watchface_get_root_layer(Watchface* watchface);
FontCollection* watchface_get_fonts(Watchface* watchface);
UIState* watchface_get_ui_state(Watchface* watchface);

StatusbarLayer* watchface_get_statusbar_layer(Watchface* watchface);
DateTimeLayer* watchface_get_date_time_layer(Watchface* watchface);
WeatherLayer* watchface_get_weather_layer(Watchface* watchface);

void watchface_set_statusbar_visible(
    Watchface* watchface, bool visible, bool animate
);
void watchface_set_weather_visible(
    Watchface* watchface, bool visible, bool animate
);

#endif
