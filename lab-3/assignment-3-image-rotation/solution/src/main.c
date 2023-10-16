#include "bmp.h"
#include "file_utils.h"
#include "image.h"
#include "transformations.h"
#include "util.h"
#include <stdio.h>

const char *HELP_MESSAGE = "Usage: image-transformation <input-image> <output-image>";

int main(int argc, char **argv) {
    if (argc != 3) {
        if (argc == 1) {
            println(HELP_MESSAGE);
            return EXIT_SUCCESS;
        } else {
            print_error(HELP_MESSAGE);
            return EXIT_FAILURE;
        }
    }


    FILE *input_file = NULL;
    const char *input_image_path = argv[1];
    const char *input_mode = "rb";
    enum open_file_status open_input_file_status =
		open_file(&input_file, input_image_path, input_mode);

    if (open_input_file_status != OPEN_FILE_STATUS_OK) {
        print_error(open_file_status_msg[open_input_file_status]);
        return EXIT_FAILURE;
    }
    DBG_MSG("main.c input file is OK");

    FILE *output_file = NULL;
    const char *output_image_path = argv[2];
    const char *output_mode = "wb";
    enum open_file_status open_output_file_status =
		open_file(&output_file, output_image_path, output_mode);

    if (open_output_file_status != OPEN_FILE_STATUS_OK) {
        close_file(input_file);
        print_error(open_file_status_msg[open_output_file_status]);
        return EXIT_FAILURE;
    }
    DBG_MSG("main.c output file is OK");

    struct image image = {.width = 0, .height = 0, .data = NULL};
    enum read_bmp_status read_input_bmp_status = from_bmp(input_file, &image);
    if (read_input_bmp_status != READ_OK) {
        print_error(read_status_msg[read_input_bmp_status]);
        clear_image(&image);
        close_file(input_file);
        close_file(output_file);
        return EXIT_FAILURE;
    }
    DBG_MSG("main.c input BMP is OK");

    struct image new_image = rotate(image);
    enum write_bmp_status write_output_bmp_status = to_bmp(output_file, &new_image);
    enum close_file_status close_input_file_status = close_file(input_file);
    enum close_file_status close_output_file_status = close_file(output_file);
    clear_image(&image);
    clear_image(&new_image);

    int exit_code = EXIT_SUCCESS;
    if (write_output_bmp_status != WRITE_OK) {
        print_error(write_status_msg[write_output_bmp_status]);
        exit_code = EXIT_FAILURE;
    }
    if (close_input_file_status != CLOSE_FILE_STATUS_OK) {
        print_error(close_status_msg[close_input_file_status]);
    }
    if (close_output_file_status != CLOSE_FILE_STATUS_OK) {
        print_error(close_status_msg[close_output_file_status]);
    }

    return exit_code;
}
