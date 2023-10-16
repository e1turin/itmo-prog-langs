#include "file_utils.h"

enum open_file_status open_file(FILE **file, const char *file_path, const char *mode) {
    if (file_path == NULL) {
        return OPEN_FILE_STATUS_NULL_FILE_PATH;
    }
    *file = fopen(file_path, mode);
    if (*file == NULL) {
        return OPEN_FILE_STATUS_ERROR;
    }
    return OPEN_FILE_STATUS_OK;
}

enum close_file_status close_file(FILE *file) {
    if (file == NULL) {
        return CLOSE_FILE_STATUS_NULL_FILE_PATH;
    }
    if (fclose(file)) {
        return CLOSE_FILE_STATUS_ERROR;
    }
    return CLOSE_FILE_STATUS_OK;
}
