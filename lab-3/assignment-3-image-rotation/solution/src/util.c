#include "util.h"

void println(const char *const str) {
    fprintf(stdout, "%s\n", str);
}

void print_error(const char *const str) {
    fprintf(stderr, "%s\n", str);
}

