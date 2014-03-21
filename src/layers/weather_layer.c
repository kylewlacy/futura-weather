#include "watchface.h"
#include "utils.h"
#include "auxiliary_layer.h"
#include "layers/weather_layer.h"

GRect default_weather_frame;
GRect screen_size;

struct WeatherLayer {
    AuxiliaryLayer* auxiliary_layer;
    
    BitmapLayer* conditions_layer;
    TextLayer* temperature_layer;
    
    GBitmap* conditions_icon;
    uint32_t conditions_resource;
};

uint32_t weather_layer_get_resource_for_conditions(
    WeatherConditions conditions
) {
    bool is_day = conditions.flags & WEATHER_CONDITION_FLAGS_IS_DAY;
    switch(conditions.code) {
        case WEATHER_CONDITIONS_CLEAR:
            return is_day ?
                RESOURCE_ID_WEATHER_CLEAR_DAY :
                RESOURCE_ID_WEATHER_CLEAR_NIGHT;
        case WEATHER_CONDITIONS_PARTLY_CLOUDY:
            return is_day ?
                RESOURCE_ID_WEATHER_PARTLY_CLOUDY_DAY :
                RESOURCE_ID_WEATHER_PARTLY_CLOUDY_NIGHT;
        case WEATHER_CONDITIONS_CLOUDY:
            return RESOURCE_ID_WEATHER_CLOUDY;
        case WEATHER_CONDITIONS_FOG:
            return RESOURCE_ID_WEATHER_FOG;
        case WEATHER_CONDITIONS_DRIZZLE:
            return RESOURCE_ID_WEATHER_DRIZZLE;
        case WEATHER_CONDITIONS_RAIN:
            return RESOURCE_ID_WEATHER_RAIN;
        case WEATHER_CONDITIONS_RAIN_SLEET:
            return RESOURCE_ID_WEATHER_RAIN_SLEET;
        case WEATHER_CONDITIONS_RAIN_SNOW:
            return RESOURCE_ID_WEATHER_RAIN_SNOW;
        case WEATHER_CONDITIONS_SLEET:
            return RESOURCE_ID_WEATHER_SLEET;
        case WEATHER_CONDITIONS_SNOW_SLEET:
            return RESOURCE_ID_WEATHER_SNOW_SLEET;
        case WEATHER_CONDITIONS_SNOW:
            return RESOURCE_ID_WEATHER_SNOW;
        case WEATHER_CONDITIONS_THUNDER:
            return RESOURCE_ID_WEATHER_THUNDER;
        case WEATHER_CONDITIONS_WIND:
            return RESOURCE_ID_WEATHER_WIND;
        case WEATHER_CONDITIONS_HOT:
            return RESOURCE_ID_WEATHER_HOT;
        case WEATHER_CONDITIONS_COLD:
            return RESOURCE_ID_WEATHER_COLD;
        case WEATHER_CONDITIONS_UNAVAILABLE:
            APP_LOG(APP_LOG_LEVEL_WARNING, "Weather conditions unavailable");
            break;
        default:
            APP_LOG(
                    APP_LOG_LEVEL_WARNING,
                    "Unknown weather condition code: %d", conditions.code
                );
            break;
    }
    
    return RESOURCE_ID_ICON_WEATHER_ERROR;
}

GRect weather_layer_get_intended_frame(bool visible) {
    GRect frame = default_weather_frame;
    
    if(!visible) {
        frame.origin.y = screen_size.size.h;
    }
    return frame;
}



WeatherLayer* weather_layer_create(Watchface* watchface) {
    // This is a global variable because the frame of `watchface->layer`
    // is the size of Pebble's screen (to remove hard-coded screen contsants)
    screen_size = layer_get_frame(watchface_get_layer(watchface));
    default_weather_frame = GRect(0, 100, screen_size.size.w, 70);
    
    WeatherLayer* weather_layer = malloc(sizeof(WeatherLayer));
    
#ifdef LIGHT_WEATHER
    bool inverted = true;
#else
    bool inverted = false;
#endif
    bool visible =
        watchface_get_ui_state(watchface)->weather_visible;
    
    weather_layer->auxiliary_layer = auxiliary_layer_create(
        watchface, visible, inverted
    );
    
    weather_layer->conditions_layer = bitmap_layer_create(GRect(9, 3, 60, 60));
    
    weather_layer->temperature_layer = text_layer_create(GRect(70, 9, 72, 70));
    text_layer_set_text_color(
        weather_layer->temperature_layer, GColorWhite
    );
    text_layer_set_background_color(
        weather_layer->temperature_layer, GColorClear
    );
    text_layer_set_font(
        weather_layer->temperature_layer,
        watchface_get_fonts(watchface)->futura_40
    );
    text_layer_set_text_alignment(
        weather_layer->temperature_layer, GTextAlignmentRight
    );
    
    layer_add_child(
        weather_layer_get_layer(weather_layer),
        bitmap_layer_get_layer(weather_layer->conditions_layer)
    );
    layer_add_child(
        weather_layer_get_layer(weather_layer),
        text_layer_get_layer(weather_layer->temperature_layer)
    );
    
    weather_layer_set_visible(weather_layer, visible);
    
    
    return weather_layer;
}

void weather_layer_destroy(WeatherLayer* weather_layer) {
    text_layer_destroy(weather_layer->temperature_layer);
    
    if(weather_layer->conditions_icon != NULL) {
        gbitmap_destroy(weather_layer->conditions_icon);
    }
    bitmap_layer_destroy(weather_layer->conditions_layer);
    
    auxiliary_layer_destroy(weather_layer->auxiliary_layer);
    
    free(weather_layer);
}



Watchface* weather_layer_get_watchface(
    WeatherLayer* weather_layer
) {
    return auxiliary_layer_get_watchface(
        weather_layer->auxiliary_layer
    );
}

Layer* weather_layer_get_layer(
    WeatherLayer* weather_layer
) {
    return auxiliary_layer_get_layer(
        weather_layer->auxiliary_layer
    );
}

Layer* weather_layer_get_root_layer(
    WeatherLayer* weather_layer
) {
    return auxiliary_layer_get_root_layer(
        weather_layer->auxiliary_layer
    );
}



void weather_layer_set_visible(
    WeatherLayer* weather_layer, bool visible
) {
    auxiliary_layer_set_visible(
        weather_layer->auxiliary_layer, visible
    );
}

Animation* weather_layer_create_animation(
    WeatherLayer* weather_layer, bool visible
) {
    return auxiliary_layer_create_animation(
        weather_layer->auxiliary_layer, visible
    );
}



void weather_layer_update_temperature_font(
    WeatherLayer* weather_layer, int temperature
) {
    if(10 <= temperature && temperature <= 99) {
        layer_set_frame(
            text_layer_get_layer(weather_layer->temperature_layer),
            GRect(70, 9+3, 72, 70)
        );
        text_layer_set_font(
            weather_layer->temperature_layer,
            watchface_get_fonts(
                weather_layer_get_watchface(weather_layer)
            )->futura_35
        );
    }
    else if(
        (0 <= temperature && temperature <= 9)
        || (-9 <= temperature && temperature <= -1)
    ) {
        layer_set_frame(
            text_layer_get_layer(weather_layer->temperature_layer),
            GRect(70, 9, 72, 70)
        );
        text_layer_set_font(
            weather_layer->temperature_layer,
            watchface_get_fonts(
                weather_layer_get_watchface(weather_layer)
            )->futura_40
        );
    }
    else if(
        (100 <= temperature)
        || (-99 <= temperature && temperature <= -10)
    ) {
        layer_set_frame(
            text_layer_get_layer(weather_layer->temperature_layer),
            GRect(70, 9+3, 72, 70)
        );
        text_layer_set_font(
            weather_layer->temperature_layer,
            watchface_get_fonts(
                weather_layer_get_watchface(weather_layer)
            )->futura_28
        );
    }
    else {
        layer_set_frame(
            text_layer_get_layer(weather_layer->temperature_layer),
            GRect(70, 9+6, 72, 70)
        );
        text_layer_set_font(
            weather_layer->temperature_layer,
            watchface_get_fonts(
                weather_layer_get_watchface(weather_layer)
            )->futura_25
        );
    }
}

void weather_layer_set_temperature(
    WeatherLayer* weather_layer, int temperature
) {
    static char temperature_text[8];
    
    snprintf(temperature_text, 8, "%d\u00B0", temperature);
    text_layer_set_text(
        weather_layer->temperature_layer, temperature_text
    );
    
    weather_layer_update_temperature_font(
        weather_layer, temperature
    );
}

void weather_layer_set_conditions(
    WeatherLayer* weather_layer, WeatherConditions conditions
) {
    if(conditions.code != WEATHER_CONDITIONS_UNAVAILABLE) {
        if(weather_layer->conditions_icon != NULL) {
            gbitmap_destroy(weather_layer->conditions_icon);
        }
        weather_layer->conditions_icon = gbitmap_create_with_resource(
            weather_layer_get_resource_for_conditions(conditions)
        );
        bitmap_layer_set_bitmap(
            weather_layer->conditions_layer, weather_layer->conditions_icon
        );
    }
}

