#include "watchface.h"
#include "utils.h"
#include "auxiliary_layer.h"
#include "layers/disconnected_layer.h"

GRect screen_size;

struct DisconnectedLayer {
    AuxiliaryLayer* auxiliary_layer;
    
    // A `TextLayer` draws a solid backgroud color
    // Yes this is excessively silly
    TextLayer* background_layer;
    
    BitmapLayer* icon_layer;
    GBitmap* icon;
};



DisconnectedLayer* disconnected_layer_create(Watchface* watchface) {
    // This is a global variable because the frame of `watchface->layer`
    // is the size of Pebble's screen (to remove hard-coded screen contsants)
    screen_size = layer_get_frame(watchface_get_layer(watchface));
    bool visible = watchface_get_ui_state(watchface)->disconnected_visible;
    
    DisconnectedLayer* disconnected_layer = malloc(sizeof(DisconnectedLayer));
    
    disconnected_layer->auxiliary_layer = auxiliary_layer_create(
        watchface, visible, true
    );
    
    GRect auxiliary_layer_frame = layer_get_frame(
        auxiliary_layer_get_layer(disconnected_layer->auxiliary_layer)
    );
    
    disconnected_layer->background_layer = text_layer_create(
        GRect(0, 0, auxiliary_layer_frame.size.w, auxiliary_layer_frame.size.h)
    );
    text_layer_set_background_color(
        disconnected_layer->background_layer, GColorBlack
    );
    
    disconnected_layer->icon_layer = bitmap_layer_create(GRect(9, 3, 60, 60));
    disconnected_layer->icon = gbitmap_create_with_resource(
        RESOURCE_ID_ICON_DISCONNECTED
    );
    
    bitmap_layer_set_bitmap(
        disconnected_layer->icon_layer, disconnected_layer->icon
    );
    
    layer_add_child(
        disconnected_layer_get_layer(disconnected_layer),
        text_layer_get_layer(disconnected_layer->background_layer)
    );
    layer_add_child(
        disconnected_layer_get_layer(disconnected_layer),
        bitmap_layer_get_layer(disconnected_layer->icon_layer)
    );
    
    disconnected_layer_set_visible(disconnected_layer, visible);
    
    return disconnected_layer;
}

void disconnected_layer_destroy(DisconnectedLayer* disconnected_layer) {
    gbitmap_destroy(disconnected_layer->icon);
    bitmap_layer_destroy(disconnected_layer->icon_layer);
    
    text_layer_destroy(disconnected_layer->background_layer);
    
    auxiliary_layer_destroy(disconnected_layer->auxiliary_layer);
    
    free(disconnected_layer);
}



Watchface* disconnected_layer_get_watchface(
    DisconnectedLayer* disconnected_layer
) {
    return auxiliary_layer_get_watchface(
        disconnected_layer->auxiliary_layer
    );
}

Layer* disconnected_layer_get_layer(
    DisconnectedLayer* disconnected_layer
) {
    return auxiliary_layer_get_layer(
        disconnected_layer->auxiliary_layer
    );
}

Layer* disconnected_layer_get_root_layer(
    DisconnectedLayer* disconnected_layer
) {
    return auxiliary_layer_get_root_layer(
        disconnected_layer->auxiliary_layer
    );
}



void disconnected_layer_set_visible(
    DisconnectedLayer* disconnected_layer, bool visible
) {
    auxiliary_layer_set_visible(
        disconnected_layer->auxiliary_layer, visible
    );
}

Animation* disconnected_layer_create_animation(
    DisconnectedLayer* disconnected_layer, bool visible
) {
    return auxiliary_layer_create_animation(
        disconnected_layer->auxiliary_layer, visible
    );
}
