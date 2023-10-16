#ifndef IMAGE_TRANSFORMER_BMP_H
#define IMAGE_TRANSFORMER_BMP_H

#include "bmp_utils.h"
#include "image.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#pragma pack(push, 1)
struct bmp_header
{
    uint16_t bfType;
    uint32_t bfileSize;
    uint32_t bfReserved;
    uint32_t bOffBits;
    uint32_t biSize;
    uint32_t biWidth;
    uint32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    uint32_t biXPelsPerMeter;
    uint32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
};
#pragma pack(pop)

static const uint16_t BMP_SIGNATURE = 0x4D42; // little-endian => 0x424D = 'BM'
static const uint32_t RESERVED = 0;
static const uint32_t BITMAP_INFO_HEADER = 40;
static const uint16_t PLANES = 1;
static const uint16_t BIT_COUNT = 24;
static const uint32_t NO_COMPRESSION = 0;
static const uint32_t PIXELS_PER_METER = 0;
static const uint32_t COLOR_AMOUNT = 0;
static const uint32_t COLOR_IMPORTANT = 0;

enum read_bmp_status {
    READ_OK,
    READ_INVALID_SIGNATURE,
    READ_INVALID_BITS,
    READ_INVALID_COMPRESSION,
    READ_INVALID_HEADER,
    READ_FILE_NULL,
    READ_MALLOC_ERROR,
    READ_ERROR
};

static const char *const read_status_msg[] = {
        [READ_OK] = "File read successfully",
        [READ_INVALID_SIGNATURE] = "Header file type is not supported",
        [READ_INVALID_BITS] = "This color depth is not supported.",
        [READ_INVALID_COMPRESSION] = "Compression is not supported.",
        [READ_INVALID_HEADER] = "The read title is incorrect",
        [READ_FILE_NULL] = "File NULL passed for reading",
        [READ_MALLOC_ERROR] = "Failed to allocate memory to store the image",
        [READ_ERROR] = "Error while reading the file",
};

enum write_bmp_status {
    WRITE_OK,
    WRITE_HEADER_ERROR,
    WRITE_ERROR,
    WRITE_FILE_NULL
};

static const char *const write_status_msg[] = {
        [WRITE_OK] = "File written successfully",
        [WRITE_HEADER_ERROR] = "Header entry error",
        [WRITE_ERROR] = "An error occurred while writing the file",
        [WRITE_FILE_NULL] = "NULL file passed for writing"
};

enum read_bmp_status from_bmp(FILE *in, struct image *img);

enum write_bmp_status to_bmp(FILE *out, struct image *img);

#endif //IMAGE_TRANSFORMER_BMP_H
