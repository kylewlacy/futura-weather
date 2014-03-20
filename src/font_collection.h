#ifndef futura_weather_font_collection_h
#define futura_weather_font_collection_h

#include "pebble.h"

typedef struct {
    GFont futura_18;
    GFont futura_25;
    GFont futura_28;
    GFont futura_35;
    GFont futura_40;
    GFont futura_53;
} FontCollection;

FontCollection* font_collection_load();
void font_collection_unload(FontCollection* font_collection);

#endif
