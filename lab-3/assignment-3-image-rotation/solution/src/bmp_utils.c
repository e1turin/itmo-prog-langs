#include "bmp_utils.h"

uint8_t calculate_padding(uint32_t width) {
    uint64_t delta = (width * sizeof(struct pixel)) % 4;
	return (4 - delta) % 4;
}

/*
 * @param `padding_arr` Array must be the size of the `padding`
 */
void create_padding_arr(uint8_t *const padding_arr, const uint8_t padding) {
    for (size_t i = 0; i < padding; i++) {
        padding_arr[i] = 0;
    }
}

uint32_t calculate_image_size(const struct image *const image, const uint8_t padding) {
    return (image->width * sizeof(struct pixel) + padding) * image->height;
}

