#include "image.h"

struct image create_image(const uint32_t width, const uint32_t height) {
    return (struct image) {
            .width = width,
            .height = height,
            .data = malloc(width * height * sizeof(struct pixel))
    };
}

void clear_image(struct image *const image) {
    if (image != NULL) {
        free(image->data);
    }
}
