//
// Created by ivtur on 17.11.2022.
//

#ifndef IMAGE_TRANSFORMER_BMP_UTILS_H
#define IMAGE_TRANSFORMER_BMP_UTILS_H

#include "image.h"
#include <inttypes.h>
#include <stdlib.h>

uint8_t calculate_padding(uint32_t width);

void create_padding_arr(uint8_t *padding_arr, uint8_t padding);

uint32_t calculate_image_size(const struct image *image, uint8_t padding);

#endif //IMAGE_TRANSFORMER_BMP_UTILS_H
