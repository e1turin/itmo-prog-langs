#ifndef IMAGE_TRANSFORMER_IMAGE_H
#define IMAGE_TRANSFORMER_IMAGE_H

#include <inttypes.h>
#include <stdlib.h>

#pragma pack(push, 1)
struct pixel {
    uint8_t b, g, r;
};
#pragma pack(pop)

struct image {
    uint64_t width, height;
    struct pixel *data;
};

struct image create_image(uint32_t width, uint32_t height);

void clear_image(struct image *image);

#endif //IMAGE_TRANSFORMER_IMAGE_H
