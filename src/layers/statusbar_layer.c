#include "utils.h"
#include "watchface.h"
#include "statusbar_layer.h"

struct StatusbarLayer {
    Watchface* watchface;
    Layer* layer;
    
    BitmapLayer* battery_icon_layer;
    GBitmap* battery_icon;
    uint32_t battery_resource;
};

GRect default_statusbar_frame;

uint32_t statusbar_layer_get_resource_for_battery_state(
    BatteryChargeState battery
) {
    if(battery.is_plugged && !battery.is_charging) {
        return RESOURCE_ID_PLUGGED_IN;
    }
    
    if(battery.charge_percent >= 100) {
        return battery.is_charging ?
        RESOURCE_ID_CHARGING_100 :
        RESOURCE_ID_BATTERY_100;
    }
    if(battery.charge_percent >= 90) {
        return battery.is_charging ?
        RESOURCE_ID_CHARGING_90 :
        RESOURCE_ID_BATTERY_90;
    }
    if(battery.charge_percent >= 80) {
        return battery.is_charging ?
        RESOURCE_ID_CHARGING_80 :
        RESOURCE_ID_BATTERY_80;
    }
    if(battery.charge_percent >= 70) {
        return battery.is_charging ?
        RESOURCE_ID_CHARGING_70 :
        RESOURCE_ID_BATTERY_70;
    }
    if(battery.charge_percent >= 60) {
        return battery.is_charging ?
        RESOURCE_ID_CHARGING_60 :
        RESOURCE_ID_BATTERY_60;
    }
    if(battery.charge_percent >= 50) {
        return battery.is_charging ?
        RESOURCE_ID_CHARGING_50 :
        RESOURCE_ID_BATTERY_50;
    }
    if(battery.charge_percent >= 40) {
        return battery.is_charging ?
        RESOURCE_ID_CHARGING_40 :
        RESOURCE_ID_BATTERY_40;
    }
    if(battery.charge_percent >= 30) {
        return battery.is_charging ?
        RESOURCE_ID_CHARGING_30 :
        RESOURCE_ID_BATTERY_30;
    }
    if(battery.charge_percent >= 20) {
        return battery.is_charging ?
        RESOURCE_ID_CHARGING_20 :
        RESOURCE_ID_BATTERY_20;
    }
    if(battery.charge_percent >= 10) {
        return battery.is_charging ?
        RESOURCE_ID_CHARGING_10 :
        RESOURCE_ID_BATTERY_10;
    }
    
    return battery.is_charging ?
    RESOURCE_ID_CHARGING_0 :
    RESOURCE_ID_BATTERY_0;
}



GRect statusbar_layer_get_intended_frame(bool visible) {
    GRect frame = default_statusbar_frame;
    
    if(!visible) {
        frame.origin.y -= frame.size.h;
    }
    return frame;
}



StatusbarLayer* statusbar_layer_create(Watchface* watchface) {
    StatusbarLayer* statusbar_layer = malloc(sizeof(StatusbarLayer));
    GSize window_size = layer_get_frame(watchface_get_layer(watchface)).size;
    
    default_statusbar_frame = GRect(0, 0, window_size.w, 15);
    statusbar_layer->watchface = watchface;
    
    statusbar_layer->layer = layer_create(default_statusbar_frame);
    
    statusbar_layer->battery_icon_layer = bitmap_layer_create(
        GRect(116, 3, 25, 11)
    );
    layer_add_child(
        statusbar_layer->layer,
        bitmap_layer_get_layer(statusbar_layer->battery_icon_layer)
    );
    
    statusbar_layer_set_visible(
        statusbar_layer, watchface_get_ui_state(watchface)->statusbar_visible
    );
    
    return statusbar_layer;
}

void statusbar_layer_destroy(StatusbarLayer* statusbar_layer) {
    if(statusbar_layer->battery_icon != NULL) {
        gbitmap_destroy(statusbar_layer->battery_icon);
    }
    bitmap_layer_destroy(statusbar_layer->battery_icon_layer);
    
    layer_destroy(statusbar_layer->layer);
    free(statusbar_layer);
}



Layer* statusbar_layer_get_layer(StatusbarLayer* statusbar_layer) {
    return statusbar_layer->layer;
}



Animation* statusbar_layer_create_animation(
    StatusbarLayer* statusbar_layer,
    bool visible
) {
    GRect frame = statusbar_layer_get_intended_frame(visible);
    PropertyAnimation* property_animation =
        property_animation_create_layer_frame(
            statusbar_layer->layer,
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

void statusbar_layer_set_visible(
    StatusbarLayer* statusbar_layer, bool visible
) {
    GRect frame = statusbar_layer_get_intended_frame(visible);
    layer_set_frame(statusbar_layer->layer, frame);
    
    layer_set_hidden(statusbar_layer->layer, !visible);
}

void statusbar_layer_handle_battery(
    StatusbarLayer* statusbar_layer, BatteryChargeState battery
) {
    uint32_t new_battery_resource =
        statusbar_layer_get_resource_for_battery_state(battery);
    if(
        new_battery_resource != statusbar_layer->battery_resource
        || statusbar_layer->battery_icon == NULL
    ) {
        statusbar_layer->battery_resource = new_battery_resource;
        
        if(statusbar_layer->battery_icon) {
            gbitmap_destroy(statusbar_layer->battery_icon);
        }
        statusbar_layer->battery_icon = gbitmap_create_with_resource(
            statusbar_layer->battery_resource
        );
        bitmap_layer_set_bitmap(
            statusbar_layer->battery_icon_layer,
            statusbar_layer->battery_icon
        );
        
        layer_mark_dirty(
            bitmap_layer_get_layer(statusbar_layer->battery_icon_layer)
        );
        layer_mark_dirty(statusbar_layer->layer);
    }
}

