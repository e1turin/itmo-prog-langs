#ifndef IMAGE_TRANSFORMER_FILE_UTILS_H
#define IMAGE_TRANSFORMER_FILE_UTILS_H

#include <inttypes.h>
#include <stdio.h>

enum open_file_status {
    OPEN_FILE_STATUS_OK,
    OPEN_FILE_STATUS_NULL_FILE_PATH,
    OPEN_FILE_STATUS_ERROR
};

static const char *const open_file_status_msg[] = {
        [OPEN_FILE_STATUS_OK] = "The file is opened successfully",
        [OPEN_FILE_STATUS_NULL_FILE_PATH] = "A NULL path was passed to open the file",
        [OPEN_FILE_STATUS_ERROR] = "An error occurred while opening the file"
};

enum close_file_status {
    CLOSE_FILE_STATUS_OK,
    CLOSE_FILE_STATUS_NULL_FILE_PATH,
    CLOSE_FILE_STATUS_ERROR
};

static const char *const close_status_msg[] = {
        [CLOSE_FILE_STATUS_OK] = "The file closed successfully",
        [CLOSE_FILE_STATUS_NULL_FILE_PATH] = "A NULL path was passed to close the file",
        [CLOSE_FILE_STATUS_ERROR] = "An error occurred while closing the file"
};

enum open_file_status open_file(FILE **file, const char *file_path, const char *mode);

enum close_file_status close_file(FILE *file);

#endif //IMAGE_TRANSFORMER_FILE_UTILS_H
