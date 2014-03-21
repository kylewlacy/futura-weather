#include "utils.h"
#include "watchface.h"

struct Watchface {
    Layer* layer;
    FontCollection* fonts;
    UIState* ui_state;
    
    StatusbarLayer* statusbar_layer;
    DateTimeLayer* date_time_layer;
    WeatherLayer* weather_layer;
    DisconnectedLayer* disconnected_layer;
};

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
    watchface->disconnected_layer = disconnected_layer_create(watchface);
    
    layer_add_child(
        watchface->layer,
        statusbar_layer_get_root_layer(watchface->statusbar_layer)
    );
    layer_add_child(
        watchface->layer,
        date_time_layer_get_root_layer(watchface->date_time_layer)
    );
    layer_add_child(
        watchface->layer,
        weather_layer_get_root_layer(watchface->weather_layer)
    );
    layer_add_child(
        watchface->layer,
        disconnected_layer_get_root_layer(watchface->disconnected_layer)
    );
    
    watchface_set_statusbar_visible(
        watchface, watchface->ui_state->statusbar_visible, false
    );
    watchface_set_weather_visible(
        watchface, watchface->ui_state->weather_visible, false
    );
    watchface_set_disconnected_visible(
        watchface, watchface->ui_state->disconnected_visible, false
    );
    
    return watchface;
}

void watchface_destroy(Watchface* watchface) {
    weather_layer_destroy(watchface->weather_layer);
    date_time_layer_destroy(watchface->date_time_layer);
    statusbar_layer_destroy(watchface->statusbar_layer);
    disconnected_layer_destroy(watchface->disconnected_layer);
    
    free(watchface->ui_state);
    font_collection_unload(watchface->fonts);
    layer_destroy(watchface->layer);
    
    free(watchface);
}



Layer* watchface_get_layer(Watchface* watchface) {
    return watchface->layer;
}

Layer* watchface_get_root_layer(Watchface* watchface) {
    return watchface->layer;
}

FontCollection* watchface_get_fonts(Watchface* watchface) {
    return watchface->fonts;
}

UIState* watchface_get_ui_state(Watchface* watchface) {
    return watchface->ui_state;
}



StatusbarLayer* watchface_get_statusbar_layer(Watchface* watchface) {
    return watchface->statusbar_layer;
}

DateTimeLayer* watchface_get_date_time_layer(Watchface* watchface) {
    return watchface->date_time_layer;
}

WeatherLayer* watchface_get_weather_layer(Watchface* watchface) {
    return watchface->weather_layer;
}

DisconnectedLayer* watchface_get_disconnected_layer(Watchface* watchface) {
    return watchface->disconnected_layer;
}



void watchface_set_statusbar_visible(
    Watchface* watchface, bool visible, bool animate
) {
    watchface->ui_state->statusbar_visible = visible;
    
    if(animate) {
        // The statusbar layer should be visible during animations. We don't
        // use `weather_layer_set_visible` because that moves the frame
        // (which we don't want)
        layer_set_hidden(
            statusbar_layer_get_layer(watchface->statusbar_layer), false
        );
        
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



void watchface_set_weather_visible(
    Watchface* watchface, bool visible, bool animate
) {
    watchface->ui_state->weather_visible = visible;
    
    if(animate) {
        // The weather layer should be visible during animations. We don't
        // use `weather_layer_set_visible` because that moves the frame
        // (which we don't want)
        // TODO: Move this out?
        layer_set_hidden(
            weather_layer_get_root_layer(watchface->weather_layer), false
        );
        
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

void watchface_set_disconnected_visible(
    Watchface* watchface, bool visible, bool animate
) {
    watchface->ui_state->disconnected_visible = visible;
    
    if(animate) {
        // The disconnected layer should be visible during animations.
        // TODO: Move this out?
        layer_set_hidden(
            disconnected_layer_get_root_layer(watchface->disconnected_layer),
            false
        );
        
        Animation* time_animation = date_time_layer_create_time_animation(
            watchface->date_time_layer
        );
        Animation* date_animation = date_time_layer_create_date_animation(
            watchface->date_time_layer
        );
        Animation* disconnected_animation = disconnected_layer_create_animation(
            watchface->disconnected_layer, visible
        );
        
        animation_set_delay(time_animation, 300);
        animation_set_delay(date_animation, 150);
        animation_set_delay(disconnected_animation, 0);
        
        animation_set_duration(time_animation, 350);
        animation_set_duration(date_animation, 350);
        animation_set_duration(disconnected_animation, 350);
        
        animation_set_curve(
            disconnected_animation,
            visible ?
                AnimationCurveEaseOut :
                AnimationCurveEaseIn
        );
        
        animation_schedule(time_animation);
        animation_schedule(date_animation);
        animation_schedule(disconnected_animation);
    }
    else {
        weather_layer_set_visible(watchface->weather_layer, visible);
        date_time_layer_update_frame(watchface->date_time_layer);
    }
}

