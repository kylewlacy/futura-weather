#include "font_collection.h"

FontCollection* font_collection_load() {
    FontCollection* font_collection = malloc(sizeof(FontCollection));
    
    font_collection->futura_18 = fonts_load_custom_font(
        resource_get_handle(RESOURCE_ID_FONT_FUTURA_18)
    );
    font_collection->futura_25 = fonts_load_custom_font(
        resource_get_handle(RESOURCE_ID_FONT_FUTURA_25)
    );
    font_collection->futura_28 = fonts_load_custom_font(
        resource_get_handle(RESOURCE_ID_FONT_FUTURA_28)
    );
    font_collection->futura_35 = fonts_load_custom_font(
        resource_get_handle(RESOURCE_ID_FONT_FUTURA_35)
    );
    font_collection->futura_40 = fonts_load_custom_font(
        resource_get_handle(RESOURCE_ID_FONT_FUTURA_40)
    );
    font_collection->futura_53 = fonts_load_custom_font(
        resource_get_handle(RESOURCE_ID_FONT_FUTURA_CONDENSED_53)
    );

    return font_collection;
}

void font_collection_unload(FontCollection* font_collection) {
    fonts_unload_custom_font(font_collection->futura_53);
    fonts_unload_custom_font(font_collection->futura_40);
    fonts_unload_custom_font(font_collection->futura_35);
    fonts_unload_custom_font(font_collection->futura_28);
    fonts_unload_custom_font(font_collection->futura_25);
    fonts_unload_custom_font(font_collection->futura_18);
    
    free(font_collection);
}
