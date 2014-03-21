#include "utils.h"

void cleanup_animation_stopped_handler(
    Animation* animation, bool finished, void* context
) {
    animation_destroy(animation);
}

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
