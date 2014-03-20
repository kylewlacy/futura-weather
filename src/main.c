#include "pebble.h"

#include "config.h"
#include "utils.h"
#include "watchface.h"
#include "preferences.h"
#include "weather.h"
#include "main.h"

static Window* window;

static Watchface* watchface;

static Preferences* prefs;
static Weather* weather;



bool has_internet_connection(Weather* weather) {
    // If weather has needed an update for a minute or over, then the
    // user likely doesn't have an internet connection (since the
    // phone should have already responded with new weather info).
    bool has_internet = !weather_needs_update(
        weather,
        prefs->weather_update_freq + 60
    );
    return has_internet;
}



void change_preferences(Preferences* old_prefs, Preferences* new_prefs) {
    // old_prefs will be NULL for initialization (app first loaded)
    if(old_prefs == NULL || old_prefs->temp_format != new_prefs->temp_format) {
        if(!weather_needs_update(weather, new_prefs->weather_update_freq)) {
            update_weather_info(weather, old_prefs != NULL);
        }
    }
    if(
        old_prefs == NULL
        || old_prefs->weather_provider != new_prefs->weather_provider
    ) {
        // TODO: Don't request weather if we don't need to
        weather_request_update();
    }
    if(old_prefs != NULL) {
        // Update the current date (but not the time) in case the
        // current translation changed
        force_tick(DAY_UNIT | MONTH_UNIT | YEAR_UNIT);
    }
    if(old_prefs == NULL || old_prefs->statusbar != new_prefs->statusbar) {
        watchface_set_statusbar_visible(
            watchface, new_prefs->statusbar, old_prefs != NULL
        );
    }
}





void update_weather_info(Weather* weather, bool animate) {
    if(weather->conditions.code != WEATHER_CONDITIONS_UNAVAILABLE) {
        int temperature = weather_convert_temperature(
            weather->temperature, prefs->temp_format
        );
        
        weather_layer_set_temperature(watchface->weather_layer, temperature);
        weather_layer_set_conditions(
            watchface->weather_layer, weather->conditions
        );
        
        watchface_set_weather_visible(watchface, true, animate);
    }
}

void update_connection_info(bool bluetooth, bool internet) {
    // TODO: Show layer in place of weather info
}







void out_sent_handler(DictionaryIterator* sent, void* context) {
}

void out_failed_handler(
    DictionaryIterator* failed, AppMessageResult reason, void* context
) {
    APP_LOG(
        APP_LOG_LEVEL_ERROR,
        "Sending message failed (reason: %d)", (int)reason
    );
}

void in_received_handler(DictionaryIterator* received, void* context) {
    Tuple* set_weather = dict_find(received, SET_WEATHER_MSG_KEY);
    Tuple* set_preferences = dict_find(received, SET_PREFERENCES_MSG_KEY);
    
    if(set_weather) {
        weather_set(weather, received);
        update_weather_info(weather, true);
        
        // Receiving weather info means we (probably) have internet!
        update_connection_info(
            bluetooth_connection_service_peek(),
            has_internet_connection(weather)
        );
    }
    
    if(set_preferences) {
        Preferences old_prefs = *prefs;
        
        preferences_set(prefs, received);
        change_preferences(&old_prefs, prefs);
        
        preferences_save(prefs);
    }
}

void in_dropped_handler(AppMessageResult reason, void* context) {
    APP_LOG(
        APP_LOG_LEVEL_ERROR,
        "Received message dropped (reason: %d)", (int)reason
    );
}



// TODO: Move this somewhere else
unsigned long get_string_between_delimiters_at_index(
    char* buffer, size_t buffer_size,
    char* str, char delim, unsigned long index
) {
    char* start = str;
    unsigned long str_length = strlen(str);
    char* end = str + str_length;
    
    memset(buffer, 0, buffer_size);
    
    while(index > 0 && start < end) {
        if(start[0] == delim) {
            index--;
        }
        start++;
    }
    
    unsigned long length = 0;
    while(start[length] != delim && start[length] != 0) {
        length++;
    }
    // Prevent reading beyond the string
    length = (length > str_length) ? str_length : length;
    
    // Make room for null character
    length++;
    
    // Prevent buffer overflow
    length = (length + 1 > buffer_size) ? buffer_size - 1 : length;
    
    if(length > 1) {
        memcpy(buffer, start, length - 1);
        buffer[length] = '\0'; // Null-terminate
    }
    else {
        buffer[0] = 0;
        length = 0;
    }
    
    return length;
}

void format_time(
    char* buffer, size_t buffer_length, struct tm* now, bool is_24h
) {
    strftime(buffer, buffer_length, is_24h ? "%H:%M" : "%I:%M", now);
}

void format_date(
    char* buffer, size_t buffer_length, struct tm* now, Preferences* prefs
) {
    int month, day_of_month, day_of_week;
    char weekday[10], month_name[10];
    
    month = now->tm_mon;
    day_of_month = now->tm_mday;
    // tm_wday defines 0 as Sunday, we want 0 as Monday
    day_of_week = ((now->tm_wday + 6) % 7);
    
    get_string_between_delimiters_at_index(
        weekday, sizeof(weekday),
        prefs->translation, ',', day_of_week + 12
    );
    get_string_between_delimiters_at_index(
        month_name, sizeof(month_name),
        prefs->translation, ',', month
    );
    
    snprintf(
        buffer, buffer_length, "%s %s %d", weekday, month_name, day_of_month
    );
    
    if(strlen(buffer) <= 0) {
        APP_LOG(
            APP_LOG_LEVEL_WARNING,
            "Failed to interpret translation %d (%s)",
            prefs->language_code, prefs->translation
        );
        strftime(buffer, buffer_length, "%a %b %d", now); // Fallback
    }
}



int main() {
    init();
    app_event_loop();
    deinit();
}

void init() {
    window = window_create();
    window_set_background_color(window, GColorBlack);
    window_set_fullscreen(window, true);
    window_set_window_handlers(window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload
    });
    
    app_message_register_inbox_received(in_received_handler);
    app_message_register_inbox_dropped(in_dropped_handler);
    app_message_register_outbox_sent(out_sent_handler);
    app_message_register_outbox_failed(out_failed_handler);
    
    const uint32_t inbound_size = app_message_inbox_size_maximum();
    const uint32_t outbound_size = app_message_outbox_size_maximum();
    app_message_open(inbound_size, outbound_size);
    
    prefs = preferences_load();
    weather = weather_load_cache();
    
    window_stack_push(window, true);
}

void window_load(Window* window) {
    Layer* window_layer = window_get_root_layer(window);
    
    UIState initial_ui_state = {
        .statusbar_visible = prefs->statusbar,
        .weather_visible = !weather_needs_update(
            weather, prefs->weather_outdated_time
        )
    };
    
    watchface = watchface_create(window, initial_ui_state);
    
    layer_add_child(window_layer, watchface->layer);
    
    change_preferences(NULL, prefs);
    
    tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
    battery_state_service_subscribe(handle_battery);
    bluetooth_connection_service_subscribe(handle_bluetooth);
    
    // "Force" an update for each subscribed service (draws
    // everything, since we're only drawing what we need)
    force_tick(ALL_UNITS);
    handle_battery(battery_state_service_peek());
    handle_bluetooth(bluetooth_connection_service_peek());
}

void window_unload(Window* window) {
    watchface_destroy(watchface);
}

void deinit() {
    window_destroy(window);
}



void handle_tick(struct tm* now, TimeUnits units_changed) {
    if(units_changed & MINUTE_UNIT) {
        static char time_text[6];
        format_time(time_text, sizeof(time_text), now, clock_is_24h_style());
        
        text_layer_set_text(
            watchface->date_time_layer->time_layer, time_text
        );
    }
    
    if(units_changed & DAY_UNIT) {
        // 18 characters should be enougth to fit the
        // most common language formats
        static char date_text[18];
        format_date(date_text, sizeof(date_text), now, prefs);
        
        text_layer_set_text(
            watchface->date_time_layer->date_layer, date_text
        );
    }
    
    bool outdated = weather_needs_update(weather, prefs->weather_outdated_time);
    if(outdated || weather_needs_update(weather, prefs->weather_update_freq)) {
        weather_request_update();
    }
    if(outdated && watchface->ui_state->weather_visible) {
        watchface_set_weather_visible(watchface, false, true);
        update_connection_info(
            bluetooth_connection_service_peek(),
            has_internet_connection(weather)
        );
    }
}

void handle_battery(BatteryChargeState battery) {
    statusbar_layer_handle_battery(watchface->statusbar_layer, battery);
}

void handle_bluetooth(bool connected) {
    update_connection_info(connected, has_internet_connection(weather));
    
    if(!connected) {
        vibes_long_pulse();
    }
}



void force_tick(TimeUnits units_changed) {
    time_t then = time(NULL);
    struct tm* now = localtime(&then);
    
    handle_tick(now, units_changed);
}
