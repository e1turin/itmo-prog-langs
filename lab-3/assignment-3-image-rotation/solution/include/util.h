#ifndef IMAGE_TRANSFORMER_UTIL_H
#define IMAGE_TRANSFORMER_UTIL_H

#include <inttypes.h>
#include <stdio.h>

#define DBG_MSG(str) printf("line=%d: " str "\n", __LINE__)

void println(const char *str);

void print_error(const char *str);

#endif //IMAGE_TRANSFORMER_UTIL_H
