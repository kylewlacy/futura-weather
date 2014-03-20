#include "utils.h"

void cleanup_animation_stopped_handler(
    Animation* animation, bool finished, void* context
) {
    animation_destroy(animation);
}
