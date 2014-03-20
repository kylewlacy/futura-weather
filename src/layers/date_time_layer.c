#include "config.h"
#include "watchface.h"
#include "utils.h"
#include "layers/date_time_layer.h"

GRect default_time_frame, default_date_frame;

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
    
    if(ui_state->statusbar_visible && ui_state->weather_visible) {
        frame.origin.y += 4;
    }
    else if(!ui_state->weather_visible) {
        frame.origin.y = 103;
    }
    
    return frame;
}



DateTimeLayer* date_time_layer_create(Watchface* watchface) {
    DateTimeLayer* date_time_layer = malloc(sizeof(DateTimeLayer));
    date_time_layer->watchface = watchface;
    
    GRect frame = layer_get_frame(watchface->layer);
    
    date_time_layer->layer = layer_create(frame);
    
    default_time_frame = GRect(0, 2, frame.size.w, frame.size.h-6);
#ifdef LIGHT_WEATHER
    default_date_frame = GRect(1, 66, frame.size.w, frame.size.h-62);
#else
    default_date_frame = GRect(1, 74, frame.size.w, frame.size.h-62);
#endif
    
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
        date_time_layer->time_layer, watchface->fonts->futura_53
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
        date_time_layer->date_layer, watchface->fonts->futura_18
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



void date_time_layer_update_frame(
    DateTimeLayer* date_time_layer
) {
    GRect time_frame = date_time_layer_get_intended_time_frame(
        date_time_layer->watchface->ui_state
    );
    GRect date_frame = date_time_layer_get_intended_date_frame(
        date_time_layer->watchface->ui_state
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
        date_time_layer->watchface->ui_state
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
        date_time_layer->watchface->ui_state
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

