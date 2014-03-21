#ifndef futura_weather_utils_h
#define futura_weather_utils_h

#include "pebble.h"

void cleanup_animation_stopped_handler(
    Animation* animation, bool finished, void* context
);
unsigned long get_string_between_delimiters_at_index(
    char* buffer, size_t buffer_size,
    char* str, char delim, unsigned long index
);

#endif
