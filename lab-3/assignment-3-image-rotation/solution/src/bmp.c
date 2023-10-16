#include "bmp.h"
#include <stdint.h>
enum read_header_status {
    READ_HEADER_STATUS_OK,
    READ_HEADER_STATUS_ERROR
};

enum read_header_status read_header(FILE *file, struct bmp_header *header) {
    if (file == NULL) {
        return READ_HEADER_STATUS_ERROR;
    }
    size_t result = fread(header, sizeof(struct bmp_header), 1, file);
    if (result != 1) {
        return READ_HEADER_STATUS_ERROR;
    }
    return READ_HEADER_STATUS_OK;
}

/*
 * Undefined Behaviour if `header` or `image` is NULL
 */
void fill_header(struct bmp_header *header, const struct image *const image) {
    const uint8_t padding = calculate_padding(image->width);
    const uint32_t image_size = calculate_image_size(image, padding);
    header->bfType = BMP_SIGNATURE;
    header->bfileSize = sizeof(struct bmp_header) + image_size;
    header->bfReserved = RESERVED;
    header->bOffBits = sizeof(struct bmp_header);
    header->biSize = BITMAP_INFO_HEADER;
    header->biWidth = image->width;
    header->biHeight = image->height;
    header->biPlanes = PLANES;
    header->biBitCount = BIT_COUNT;
    header->biCompression = NO_COMPRESSION;
    header->biSizeImage = image_size;
    header->biXPelsPerMeter = PIXELS_PER_METER;
    header->biYPelsPerMeter = PIXELS_PER_METER;
    header->biClrUsed = COLOR_AMOUNT;
    header->biClrImportant = COLOR_IMPORTANT;
}

enum read_bmp_status from_bmp(FILE *in, struct image *img) {
    if (in == NULL) {
        return READ_FILE_NULL;
    }

    struct bmp_header header;
    enum read_header_status result = read_header(in, &header);

    if (result == READ_HEADER_STATUS_ERROR) {
        return READ_INVALID_HEADER;
    } else if (header.bfType != BMP_SIGNATURE) {
        return READ_INVALID_SIGNATURE;
    } else if (header.biBitCount != BIT_COUNT) {
        return READ_INVALID_BITS;
    } else if (header.biCompression != NO_COMPRESSION) {
        return READ_INVALID_COMPRESSION;
    }

    *img = create_image(header.biWidth, header.biHeight);
    if (img->data == NULL) {
        return READ_MALLOC_ERROR;
    }

    const uint8_t padding = calculate_padding(img->width);
    if (fseek(in, header.bOffBits, SEEK_SET)) {
        return READ_ERROR;
    }

    for (size_t i = 0; i < header.biHeight; i++) {
        if (fread(img->data + (img->width) * i, (img->width) * sizeof(struct pixel), 1, in) != 1
            || fseek(in, padding, SEEK_CUR)) {
            clear_image(img);
            return READ_ERROR;
        }
    }
    return READ_OK;
}

enum write_bmp_status to_bmp(FILE *out, struct image *img) {
    if (out == NULL || img == NULL) {
        return WRITE_FILE_NULL;
    }
    struct bmp_header header;
    fill_header(&header, img);
    if (fwrite(&header, sizeof(struct bmp_header), 1, out) != 1
        || fseek(out, header.bOffBits, SEEK_SET)) {
        return WRITE_HEADER_ERROR;
    }
    const uint8_t padding = calculate_padding(img->width);
	uint8_t padding_array[padding];
	create_padding_arr(padding_array, padding);
    for (size_t i = 0; i < img->height; ++i) {
        if (fwrite(img->data + img->width * i, img->width * sizeof(struct pixel), 1, out) != 1
            || fwrite(padding_array, padding, 1, out) != 1) {
            return WRITE_ERROR;
        }
    }
    return WRITE_OK;
}
