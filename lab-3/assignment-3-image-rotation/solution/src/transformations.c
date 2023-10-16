#include "transformations.h"
#include "image.h"

struct image rotate(const struct image source) {
    struct image new_image = create_image(source.height, source.width);
    for (size_t i = 0; i < source.height; ++i) {
        for (size_t j = 0; j < source.width; j++) {
            new_image.data[j * source.height + (source.height - 1 - i)] = source.data[i * (source.width) + j];
        }
    }
    return new_image;
}
