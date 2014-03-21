#include "config.h"
#include "watchface.h"
#include "utils.h"
#include "layers/date_time_layer.h"

struct DateTimeLayer {
    Watchface* watchface;
    Layer* layer;
    TextLayer* time_layer;
    TextLayer* date_layer;
};

GRect default_time_frame, default_date_frame, nudged_up_date_frame;

GRect date_time_layer_get_intended_time_frame(
    UIState* ui_state
) {
    GRect frame = default_time_frame;
    
    if(ui_state->statusbar_visible && ui_state->weather_visible) {
        frame.origin.y += 8;
    }
    else if(!ui_state->weather_visible) {
        frame.origin.y = 30;
    }
    
    return frame;
}

GRect date_time_layer_get_intended_date_frame(
    UIState* ui_state
) {
    GRect frame = default_date_frame;
    
    bool auxiliary_visible = ui_state->weather_visible
                          || ui_state->disconnected_visible;
    if(ui_state->disconnected_visible) {
        frame = nudged_up_date_frame;
    }
    if(ui_state->statusbar_visible && auxiliary_visible) {
        frame.origin.y += 4;
    }
    else if(!auxiliary_visible) {
        frame.origin.y = 103;
    }
    
    return frame;
}

void date_time_layer_format_time(
    char* buffer, size_t buffer_length, struct tm* now, bool is_24h
) {
    strftime(buffer, buffer_length, is_24h ? "%H:%M" : "%I:%M", now);
}

void date_time_layer_format_date(
    char* buffer, size_t buffer_length, struct tm* now, char* translation
) {
    int month, day_of_month, day_of_week;
    char weekday[10], month_name[10];
    
    month = now->tm_mon;
    day_of_month = now->tm_mday;
    // tm_wday defines 0 as Sunday, we want 0 as Monday
    day_of_week = ((now->tm_wday + 6) % 7);
    
    get_string_between_delimiters_at_index(
        weekday, sizeof(weekday),
        translation, ',', day_of_week + 12
    );
    get_string_between_delimiters_at_index(
        month_name, sizeof(month_name),
        translation, ',', month
    );
    
    snprintf(
        buffer, buffer_length, "%s %s %d", weekday, month_name, day_of_month
    );
    
    if(strlen(buffer) <= 0) {
        APP_LOG(
            APP_LOG_LEVEL_WARNING,
            "Failed to interpret translation (%s)",
            translation
        );
        strftime(buffer, buffer_length, "%a %b %d", now); // Fallback
    }
}



DateTimeLayer* date_time_layer_create(Watchface* watchface) {
    DateTimeLayer* date_time_layer = malloc(sizeof(DateTimeLayer));
    date_time_layer->watchface = watchface;
    
    GRect frame = layer_get_frame(watchface_get_layer(watchface));
    
    date_time_layer->layer = layer_create(frame);
    
    default_time_frame = GRect(0, 2, frame.size.w, frame.size.h-6);
    default_date_frame = GRect(1, 74, frame.size.w, frame.size.h-62);
    nudged_up_date_frame = GRect(
        default_date_frame.origin.x, default_date_frame.origin.y - 8,
        default_date_frame.size.w,   default_date_frame.size.h
    );
    if(HAS_LIGHT_WEATHER) {
        default_date_frame = nudged_up_date_frame;
    }
    
    date_time_layer->time_layer = text_layer_create(default_time_frame);
    date_time_layer->date_layer = text_layer_create(default_date_frame);
    
    text_layer_set_text_alignment(
        date_time_layer->time_layer, GTextAlignmentCenter
    );
    text_layer_set_background_color(
        date_time_layer->time_layer, GColorClear
    );
    text_layer_set_text_color(
        date_time_layer->time_layer, GColorWhite
    );
    text_layer_set_font(
        date_time_layer->time_layer, watchface_get_fonts(watchface)->futura_53
    );
    text_layer_set_text(date_time_layer->time_layer, "(null)");
    
    text_layer_set_text_alignment(
        date_time_layer->date_layer, GTextAlignmentCenter
    );
    text_layer_set_background_color(
        date_time_layer->date_layer, GColorClear
    );
    text_layer_set_text_color(
        date_time_layer->date_layer, GColorWhite
    );
    text_layer_set_font(
        date_time_layer->date_layer, watchface_get_fonts(watchface)->futura_18
    );
    
    layer_add_child(
        date_time_layer->layer,
        text_layer_get_layer(date_time_layer->time_layer)
    );
    layer_add_child(
        date_time_layer->layer,
        text_layer_get_layer(date_time_layer->date_layer)
    );
    
    date_time_layer_update_frame(date_time_layer);
    
    return date_time_layer;
}

void date_time_layer_destroy(DateTimeLayer* date_time_layer) {
    text_layer_destroy(date_time_layer->date_layer);
    text_layer_destroy(date_time_layer->time_layer);
    
    layer_destroy(date_time_layer->layer);
    
    free(date_time_layer);
}

void date_time_layer_update_date(
    DateTimeLayer* date_time_layer,
    struct tm* now, char* translation
) {
    static char date_text[18];
    date_time_layer_format_date(date_text, sizeof(date_text), now, translation);
    text_layer_set_text(date_time_layer->date_layer, date_text);
}

void date_time_layer_update_time(
    DateTimeLayer* date_time_layer,
    struct tm* now, bool is_24h
) {
    static char time_text[6];
    date_time_layer_format_time(time_text, sizeof(time_text), now, is_24h);
    text_layer_set_text(date_time_layer->time_layer, time_text);
}


Watchface* date_time_layer_get_watchface(
    DateTimeLayer* date_time_layer
) {
    return date_time_layer->watchface;
}

Layer* date_time_layer_get_layer(
    DateTimeLayer* date_time_layer
) {
    return date_time_layer->layer;
}

Layer* date_time_layer_get_root_layer(
    DateTimeLayer* date_time_layer
) {
    return date_time_layer->layer;
}



void date_time_layer_update_frame(
    DateTimeLayer* date_time_layer
) {
    GRect time_frame = date_time_layer_get_intended_time_frame(
        watchface_get_ui_state(date_time_layer->watchface)
    );
    GRect date_frame = date_time_layer_get_intended_date_frame(
        watchface_get_ui_state(date_time_layer->watchface)
    );

    layer_set_frame(
        text_layer_get_layer(date_time_layer->time_layer), time_frame
    );
    layer_set_frame(
        text_layer_get_layer(date_time_layer->date_layer), date_frame
    );
}

Animation* date_time_layer_create_time_animation(
    DateTimeLayer* date_time_layer
) {
    GRect frame = date_time_layer_get_intended_time_frame(
        watchface_get_ui_state(date_time_layer->watchface)
    );
    
    PropertyAnimation* property_animation =
        property_animation_create_layer_frame(
            text_layer_get_layer(date_time_layer->time_layer),
            NULL, &frame
        );
    
    animation_set_handlers(
        &property_animation->animation,
        (AnimationHandlers) {
            .stopped = cleanup_animation_stopped_handler
        },
        NULL
    );
    animation_set_duration(&property_animation->animation, 350);
    
    return &property_animation->animation;
}

Animation* date_time_layer_create_date_animation(
    DateTimeLayer* date_time_layer
) {
    GRect frame = date_time_layer_get_intended_date_frame(
        watchface_get_ui_state(date_time_layer->watchface)
    );
    
    PropertyAnimation* property_animation =
        property_animation_create_layer_frame(
            text_layer_get_layer(date_time_layer->date_layer),
            NULL, &frame
        );
    
    animation_set_handlers(
        &property_animation->animation,
        (AnimationHandlers) {
            .stopped = cleanup_animation_stopped_handler
        },
        NULL
    );
    animation_set_duration(&property_animation->animation, 350);
    
    return &property_animation->animation;
}

