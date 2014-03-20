#include "utils.h"
#include "watchface.h"

Watchface* watchface_create(Window* window, UIState initial_ui_state) {
    Watchface* watchface = malloc(sizeof(Watchface));
    
    Layer* window_layer = window_get_root_layer(window);
    GRect window_frame = layer_get_frame(window_layer);
    
    watchface->layer = layer_create(window_frame);
    watchface->fonts = font_collection_load();
    
    watchface->ui_state = malloc(sizeof(UIState));
    memcpy(watchface->ui_state, &initial_ui_state, sizeof(UIState));
    
    watchface->statusbar_layer = statusbar_layer_create(watchface);
    watchface->date_time_layer = date_time_layer_create(watchface);
    watchface->weather_layer = weather_layer_create(watchface);
    
    layer_add_child(
        watchface->layer, watchface->statusbar_layer->layer
    );
    layer_add_child(
        watchface->layer, watchface->date_time_layer->layer
    );
    layer_add_child(
        watchface->layer, watchface->weather_layer->layer
    );
    
    watchface_set_statusbar_visible(
        watchface, watchface->ui_state->statusbar_visible, false
    );
    watchface_set_weather_visible(
        watchface, watchface->ui_state->weather_visible, false
    );
    
    return watchface;
}

void watchface_destroy(Watchface* watchface) {
    weather_layer_destroy(watchface->weather_layer);
    date_time_layer_destroy(watchface->date_time_layer);
    statusbar_layer_destroy(watchface->statusbar_layer);
    
    free(watchface->ui_state);
    font_collection_unload(watchface->fonts);
    layer_destroy(watchface->layer);
    
    free(watchface);
}



void watchface_set_statusbar_visible(
    Watchface* watchface, bool visible, bool animate
) {
    watchface->ui_state->statusbar_visible = visible;
    
    if(animate) {
        layer_set_hidden(watchface->statusbar_layer->layer, false);
        
        // TODO: Hide statusbar after animation
        Animation* statusbar_animation = statusbar_layer_create_animation(
            watchface->statusbar_layer, visible
        );
        Animation* time_animation = date_time_layer_create_time_animation(
            watchface->date_time_layer
        );
        Animation* date_animation = date_time_layer_create_date_animation(
            watchface->date_time_layer
        );
        
        animation_set_delay(statusbar_animation, 0);
        animation_set_delay(time_animation, 100);
        animation_set_delay(date_animation, 200);
        
        animation_set_duration(statusbar_animation, 250);
        animation_set_duration(time_animation, 250);
        animation_set_duration(date_animation, 250);
        
        animation_set_curve(
            statusbar_animation,
            visible ?
                AnimationCurveEaseIn :
                AnimationCurveEaseOut
        );
        
        animation_schedule(statusbar_animation);
        animation_schedule(time_animation);
        animation_schedule(date_animation);
    }
    else {
        statusbar_layer_set_visible(
            watchface->statusbar_layer, visible
        );
        date_time_layer_update_frame(watchface->date_time_layer);
    }
}


//void set_weather_visible_animation_stopped_handler(
//    Animation* animation, bool finished, void* context
//) {
//    if(finished) {
//        weather_layer_set_visible(weather_layer, context != 0);
//    }
//    cleanup_animation_stopped_handler(animation, finished, context);
//}

void watchface_set_weather_visible(
    Watchface* watchface, bool visible, bool animate
) {
    watchface->ui_state->weather_visible = visible;
    
    if(animate) {
        // The weather layer should be visible during animations
        // NOTE: We don't use `weather_layer_set_visible` because that
        // moves the frame (which we don't want)
        // TODO: Move this out?
        layer_set_hidden(watchface->weather_layer->layer, false);
        
        Animation* time_animation = date_time_layer_create_time_animation(
            watchface->date_time_layer
        );
        Animation* date_animation = date_time_layer_create_date_animation(
            watchface->date_time_layer
        );
        Animation* weather_animation = weather_layer_create_animation(
            watchface->weather_layer, visible
        );
        
        animation_set_delay(time_animation, 300);
        animation_set_delay(date_animation, 150);
        animation_set_delay(weather_animation, 0);
        
        animation_set_duration(time_animation, 350);
        animation_set_duration(date_animation, 350);
        animation_set_duration(weather_animation, 500);
        
        animation_set_curve(
            weather_animation,
            visible ?
                AnimationCurveEaseOut :
                AnimationCurveEaseIn
        );
        
        animation_schedule(time_animation);
        animation_schedule(date_animation);
        animation_schedule(weather_animation);
    }
    else {
        weather_layer_set_visible(watchface->weather_layer, visible);
        date_time_layer_update_frame(watchface->date_time_layer);
    }
}

