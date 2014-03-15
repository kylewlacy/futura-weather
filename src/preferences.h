#ifndef futura_weather_preferences_h
#define futura_weather_preferences_h

#include "pebble.h"

typedef struct {
    TempFormat temp_format;
    time_t weather_update_freq;
    bool statusbar;
    int weather_provider;
    time_t weather_outdated_time;
    int language_code;
    char translation[256];
    
    // Compile-time options (see `preferences.c` for initialization)
    const struct {
        bool light_weather;
    } flags;
} Preferences;

Preferences* preferences_load();
bool preferences_save(Preferences* prefs);
void preferences_send(Preferences* prefs);
void preferences_set(Preferences* prefs, DictionaryIterator* iter);

#endif
