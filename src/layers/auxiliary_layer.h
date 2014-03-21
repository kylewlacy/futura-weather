#ifndef futura_weather_auxiliary_layer_h
#define futura_weather_auxiliary_layer_h

#include "pebble.h"
#include "ui_state.h"

struct Watchface;

typedef struct AuxiliaryLayer AuxiliaryLayer;

AuxiliaryLayer* auxiliary_layer_create(
    struct Watchface* watchface, bool inverted, bool visible
);
void auxiliary_layer_destroy(AuxiliaryLayer* auxiliary_layer);

Watchface* auxiliary_layer_get_watchface(
    AuxiliaryLayer* auxiliary_layer
);
Layer* auxiliary_layer_get_layer(
    AuxiliaryLayer* auxiliary_layer
);
Layer* auxiliary_layer_get_root_layer(
    AuxiliaryLayer* auxiliary_layer
);

void auxiliary_layer_set_visible(
    AuxiliaryLayer* auxiliary_layer, bool visible
);
Animation* auxiliary_layer_create_animation(
    AuxiliaryLayer* auxiliary_layer, bool visible
);

#endif
