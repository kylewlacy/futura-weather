#include "watchface.h"
#include "utils.h"
#include "layers/auxiliary_layer.h"

struct AuxiliaryLayer {
    struct Watchface* watchface;
    Layer* layer;
    Layer* root_layer;
    
    bool inverted;
    
    InverterLayer* inverter_layer;
};

GRect default_auxiliary_frame, screen_size;

GRect auxiliary_layer_get_intended_frame(bool visible) {
    GRect frame = default_auxiliary_frame;
    
    if(!visible) {
        frame.origin.y = screen_size.size.h;
    }
    return frame;
}



AuxiliaryLayer* auxiliary_layer_create(
    Watchface* watchface, bool inverted, bool visible
) {
    // This is a global variable because the frame of
    // `auxiliary->root_layer` is the size of Pebble's
    // screen (to remove hard-coded screen contsants)
    screen_size = layer_get_frame(watchface_get_layer(watchface));
    default_auxiliary_frame = GRect(0, 100, screen_size.size.w, 70);
    
    AuxiliaryLayer* auxiliary_layer = malloc(sizeof(AuxiliaryLayer));
    auxiliary_layer->watchface = watchface;
    
    auxiliary_layer->root_layer = layer_create(default_auxiliary_frame);
    
    GRect child_layer = GRect(
        0,                              0,
        default_auxiliary_frame.size.w, default_auxiliary_frame.size.h
    );
    
    auxiliary_layer->layer = layer_create(child_layer);
    
    layer_add_child(
        auxiliary_layer->root_layer,
        auxiliary_layer->layer
    );
    
    if(inverted) {
        auxiliary_layer->inverter_layer = inverter_layer_create(child_layer);
        
        layer_add_child(
            auxiliary_layer->root_layer,
            inverter_layer_get_layer(
                auxiliary_layer->inverter_layer
            )
        );
    }
    
    auxiliary_layer_set_visible(
        auxiliary_layer,
        watchface_get_ui_state(watchface)->weather_visible
    );
    
    
    return auxiliary_layer;
}

void auxiliary_layer_destroy(AuxiliaryLayer* auxiliary_layer) {
    if(auxiliary_layer->inverter_layer != NULL) {
        inverter_layer_destroy(auxiliary_layer->inverter_layer);
    }
    
    layer_destroy(auxiliary_layer->layer);
    
    free(auxiliary_layer);
}

Watchface* auxiliary_layer_get_watchface(
    AuxiliaryLayer* auxiliary_layer
) {
    return auxiliary_layer->watchface;
}

Layer* auxiliary_layer_get_layer(
    AuxiliaryLayer* auxiliary_layer
) {
    return auxiliary_layer->layer;
}

Layer* auxiliary_layer_get_root_layer(
    AuxiliaryLayer* auxiliary_layer
) {
    return auxiliary_layer->root_layer;
}



struct SetVisibleAnimationContext {
    AuxiliaryLayer* auxiliary_layer;
    bool visible;
};

void auxiliary_layer_set_visible_animation_stopped_handler(
    Animation* animation, bool finished, void* context
) {
    struct SetVisibleAnimationContext* ctx =
        (struct SetVisibleAnimationContext*)context;
    
    if(finished) {
        auxiliary_layer_set_visible(ctx->auxiliary_layer, ctx->visible);
    
        cleanup_animation_stopped_handler(animation, finished, context);
        free(ctx);
    }
}

void auxiliary_layer_set_visible(
    AuxiliaryLayer* auxiliary_layer, bool visible
) {
    GRect frame = auxiliary_layer_get_intended_frame(visible);
    
    layer_set_frame(auxiliary_layer->root_layer, frame);
    layer_set_hidden(auxiliary_layer->root_layer, !visible);
}

Animation* auxiliary_layer_create_animation(
    AuxiliaryLayer* auxiliary_layer, bool visible
) {
    GRect frame = auxiliary_layer_get_intended_frame(visible);
    
    PropertyAnimation* property_animation =
        property_animation_create_layer_frame(
            auxiliary_layer->root_layer,
            NULL, &frame
        );
    
    struct SetVisibleAnimationContext* context = malloc(
        sizeof(struct SetVisibleAnimationContext)
    );
    
    context->auxiliary_layer = auxiliary_layer;
    context->visible = visible;
    
    animation_set_handlers(
        &property_animation->animation,
        (AnimationHandlers) {
            .stopped = auxiliary_layer_set_visible_animation_stopped_handler
        },
        context
    );
    animation_set_duration(&property_animation->animation, 500);
    
    return &property_animation->animation;
}
